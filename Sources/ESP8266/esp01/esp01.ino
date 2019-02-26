#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <FirebaseArduino.h>
#include <ESP_Messenger.h>
#include <ESP_ErrorManager.h>

#define _DEBUG_ 0
#ifdef _DEBUG_
#define WIFI_SSID "TeleCentro-3964"
#define WIFI_PASSWORD "271097533"
#else
#define WIFI_SSID "Cerberus"
#define WIFI_PASSWORD "tormentasolar"
#endif
#define FIREBASE_HOST "cerberus-rayquendev.firebaseio.com"
#define FIREBASE_AUTH "MTxmcq7dCu9tGNuvAOGZqm9wOnpLXtbLMtXbUym9"

#define LED 2

unsigned int wifiStatusPrevious;
unsigned int wifiStatus;
bool wifiStatusChanged;
bool wifiConnectTry;
ESPMessenger messenger;
ESPErrorManager errorManager;

#define FB_DISCONNECT 0
#define FB_RECONNECT 1
#define FB_CONFIGURE 2
#define FB_RUN 3

unsigned long currentTime;
unsigned long instantFirebaseReconnection;
const unsigned long lapseFirebaseReconnection = 2000;

unsigned int firebaseStatus = FB_DISCONNECT;

class Debug{
  public:
  void print(String a){
    Serial.print(a);
  }
  void println(String a){
    Serial.println(a);
  }
};

Debug debug;

class StreamEvent{
  public:
    bool connectionReady;
    bool streamReady;
    String path;
    FirebaseArduino firebase;
  
    StreamEvent(const String& host, const String& auth, String node){
      path = node;
      connectionReady = false;
      streamReady = false;
      firebase.begin(host, auth);
    }

    bool checkConnection(){
      firebase.getString("/test");
      if ( firebase.success() ){
        return true;
      }else{
        return false;
      }
    }

    bool streamBegin(){
      firebase.stream( path );
      if ( firebase.success() ){
        return true;
      }else{
        return false;
      }
    }
};

#define CANT_STREAMS 1
StreamEvent *streamEvents[CANT_STREAMS];

unsigned long eventCountOutCicle = 0;
unsigned int eventCountInnerCicle = 0;
String eventType,eventPath,eventData;

void setup()
{
  Serial.begin(115200);
  
  pinMode (LED, OUTPUT);
  digitalWrite (LED, LOW);

  wifiStatusChanged = false;
  wifiStatus = WL_DISCONNECTED;
  wifiStatusPrevious = WL_DISCONNECTED;
  wifiConnectTry=false;

  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);

  messenger.setErrorManager(&errorManager);
  messenger.setPort ( &Serial );

  delay(5000);

  streamEvents[0] = new StreamEvent(FIREBASE_HOST, FIREBASE_AUTH, "/preferredStatus");

  eventCountOutCicle = 0;
  eventCountInnerCicle = 0;
}

void loop () {
  currentTime = millis();

  wifiStatusPrevious = wifiStatus;
  wifiStatus = WiFi.status();
  if ( wifiStatusPrevious != wifiStatus ){
    wifiStatusChanged = true;
  }else{
    wifiStatusChanged = false;
  }

  switch ( wifiStatus ){
    case WL_CONNECTED:{
      if ( wifiStatusChanged ){
        //debug.println("WL_CONNECTED");
        firebaseStatus = FB_RECONNECT;
        instantFirebaseReconnection = currentTime + lapseFirebaseReconnection;
      }
      else{
        switch ( firebaseStatus ){
          case FB_DISCONNECT:{
            firebaseStatus = FB_RECONNECT;
            instantFirebaseReconnection = currentTime + lapseFirebaseReconnection;
          }break;
          case FB_RECONNECT:{
            if ( currentTime - instantFirebaseReconnection > lapseFirebaseReconnection ){
              debug.println("Reconectando Firebase...");
              debug.println("Chequeando conexion...");
              bool connectionReady = true;
              for (int s=0; s<CANT_STREAMS; s++){
                if ( !streamEvents[s]->connectionReady ){
                  //debug.print(streamEvents[s]->path+"... ");

                  if ( streamEvents[s]->checkConnection() ){
                    streamEvents[s]->connectionReady = true;
                    //debug.println("Ok");
                  }else{
                    streamEvents[s]->connectionReady = false;
                    connectionReady = false;
                    //debug.println("Fail");
                  }
                }
              }

              if ( connectionReady ){
                firebaseStatus = FB_CONFIGURE;
                debug.println("Configurando...");
              }else{
                firebaseStatus = FB_RECONNECT;
                instantFirebaseReconnection = currentTime;
                //debug.println("Reintentando...");
              }
            }
          }break;
          case FB_CONFIGURE:{
            bool streamReady=true;
            for (int s=0; s<CANT_STREAMS; s++){
              if ( !streamEvents[s]->streamReady ){
                debug.print(streamEvents[s]->path+"... ");

                if ( streamEvents[s]->streamBegin() ){
                  debug.println("Ok");
                  streamEvents[s]->streamReady = true;
                }else{
                  debug.println("Error");
                  streamEvents[s]->streamReady = false;
                  streamReady = false;
                }
              }
            }

            if ( streamReady ){
              debug.println( "Conexion establecida con exito!!!" );
              firebaseStatus = FB_RUN;
            }else{
              debug.println( "Aún falta configurar streams" );
            }
          }break;
          case FB_RUN:{
            for ( int s=0; s<CANT_STREAMS; s++){
              while ( streamEvents[s]->firebase.available() ){
                FirebaseObject event = streamEvents[s]->firebase.readEvent();
                eventType = event.getString("type");
  
                if ( eventType!="" ){
                  eventPath = event.getString("path");
                  eventData = event.getString("data");
                  
                  eventCountInnerCicle++;
                  if (eventCountInnerCicle == 1){
                    eventCountOutCicle++;
                  }
                  //debug.println("");
                  //debug.println("["+String(s)+":"+String(eventCountOutCicle)+":"+String(eventCountInnerCicle)+"] Nuevo Evento");
                  //debug.println("   type: "+eventType);
                  //debug.println("   path: "+eventPath);
                  //debug.println("   path2: "+streamEvents[s]->path);
                  //debug.println("   data: "+eventData);

                  if ( eventData!="" ){
                    if ( eventPath == "/system/ping"){
                      messenger.sendPing();
                    }
                    if ( eventPath == "/power" ){
                      messenger.sendSetPower(eventData);
                    }
                    
                    if ( eventPath.indexOf("/actuators/")>=0 ){
                      unsigned int id = String(eventPath.charAt( eventPath.length()-1 )).toInt();
                      //debug.println("Enviando: " + String(id));
                      messenger.sendSetActuator(id,eventData);
                    }
                    if ( eventPath.indexOf("/sensors/")>=0 ){
                      unsigned int id = String(eventPath.charAt( eventPath.length()-1 )).toInt();
                      messenger.sendSetSensor(id,eventData);
                    }
                  }
                }
              }
            }
            eventCountInnerCicle=0;
          }break;
        }
      }
    }break;
    default:{
      if ( wifiStatusChanged ){
        switch ( wifiStatus ){
          case WL_NO_SSID_AVAIL:{
            //debug.println("WL_NO_SSID_AVAIL");
            errorManager.newError(WIFI_NO_SSID_AVAIL);
          }break;
          case WL_CONNECT_FAILED:{
            //debug.println("WL_CONNECT_FAILED");
            errorManager.newError(WIFI_CONNECT_FAILED);
          }break;
          case WL_IDLE_STATUS:{
            //debug.println("WL_IDLE_STATUS");
            errorManager.newError(WIFI_IDLE_STATUS);
          }break;
          case WL_DISCONNECTED:{
            //debug.println("WL_DISCONNECTED");
            errorManager.newError(WIFI_DISCONNECTED);
          }break;
        }
        wifiConnectTry=false;
      }else{
        if ( !wifiConnectTry ){
          wifiConnectTry = true;
          WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
          //debug.println("Intando conectarme otra vez...");
        }
      }
    }break;
  }

  String comand;
  bool newComand = false;
  while( Serial.available ){
    //estoy recibiendo un comando extra
    char c = Serial.read();
    if ( c=='\n' || c=='\r' ){
    }else{
      if ( c==';' ){
        newComand=true;
      }else{
        comand+=c;
      }
    }

    if ( newComand ){

      //decodifico el comando
      if ( newComand.indexOf("A=")>=0 ){
        debug.println("Actuator update");
        int coma = newComand.indexOf(",");
        if ( coma>=0 ){
          String id = newComand.substring(2,coma);
          String value = newComand.substring(coma);
          debug.println("  id: "+id);
          debug.println("  value: "+value);
          streamEvents[0].firebase.set("/status/actuators/"+id,value);
        }else{
          debug.println("  Error no coma");
        }
      }

      //preparo para el siguiente comando
      newComand = false;
      comand = "";
    }
  }
  
/*
  if ( errorManager.errorsAvailable() ){

    for ( int e=0; e<ERRORS_ESP_TOTAL; e++ ){
        if ( errorManager.errors[e]->errorAvailable() ){
            if ( messenger.sendError(errorManager.errors[e]->code, errorManager.errors[e]->quantity) ){
              errorManager.errors[e]->clear();
            }
        }
    }
    

  }*/

}
