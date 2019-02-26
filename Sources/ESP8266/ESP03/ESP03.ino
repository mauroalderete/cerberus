#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <FirebaseArduino.h>

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

#define FB_DISCONNECT 0
#define FB_RECONNECT 1
#define FB_CONFIGURE 2
#define FB_RUN 3

unsigned long currentTime;
unsigned long instantFirebaseReconnection;
const long lapseFirebaseReconnection = 2000;

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

String eventType,eventPath,eventData;
unsigned int continuousErroresData;
unsigned int continuousErroresReconnect;

String command;
String commandPartial;
bool newCommand = false;
bool commandSent = true;


/*
 * Genera un parpadeo del led del ESP01, para poder tener un feedback rapido
 * sobre la instancia y estado del esp
 * Posee 5 modos de parpadeo claramente distinguidos. Los significados seran:
 * verySlow:  funciona todo correctamente
 * Slow:  
 * Normal:  se conecto a wifi, pero falta firebase
 * Fast:  
 * veryFast:  reiniciando, no hay conexion wifi ni tampoco con firebase
 */
class LedMonitor{
  private:
  unsigned int led;
  unsigned long currentTime;
  unsigned long lapse;
  unsigned int lapseHigh;
  unsigned int lapseLow;
  
  public:
  LedMonitor(unsigned int pin){
    led = pin;
    pinMode(led, OUTPUT);
    normal();
    lapse = millis();
  }

  void setBlink(unsigned int high, unsigned int low){
    lapseHigh = high;
    lapseLow = low;
  }
  void verySlow(){
    setBlink(4000,4000);
  }

  void slow(){
    setBlink(2000,2000);
  }

  void normal(){
    setBlink(1000,1000);
  }

  void fast(){
    setBlink(500,500);
  }

  void veryFast(){
    setBlink(200,200);
  }

  void Loop(){
    currentTime = millis();
    if ( currentTime - lapse > lapseHigh + lapseLow ){
      digitalWrite(led, HIGH);
      lapse = currentTime;
    }else{
      if ( currentTime - lapse > lapseHigh ){
        digitalWrite(led, LOW);
      }
    }
  }
};

LedMonitor led(2);

bool checkConnection(){
  Firebase.getString("/test");
  if ( Firebase.success() ){
    return true;
  }else{
    return false;
  }
}

bool streamBegin( String path ){
  Firebase.stream( path );
  if ( Firebase.success() ){
    return true;
  }else{
    return false;
  }
}

void setup()
{
  Serial.begin(115200);

  led.veryFast();

  /*
   * Preparo los elementos para controlar la conexion wifi
   */
  wifiStatusChanged = false;
  wifiStatus = WL_DISCONNECTED;
  wifiStatusPrevious = WL_DISCONNECTED;
  wifiConnectTry=false;

  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);

  delay(2000);

  Firebase.begin(FIREBASE_HOST,FIREBASE_AUTH);

  Serial.print("ESPRST;");
}

void loop () {
  currentTime = millis();
  led.Loop();

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
        ////debug.println("WL_CONNECTED");
        led.normal();
        firebaseStatus = FB_RECONNECT;
        instantFirebaseReconnection = millis() + lapseFirebaseReconnection;
      }
      else{
        switch ( firebaseStatus ){
          case FB_RECONNECT:{
            if ( millis() - instantFirebaseReconnection > lapseFirebaseReconnection ){
              //debug.print("FB_RECONNECT... ");
              if ( checkConnection() ){
                if ( streamBegin("/newPreferredStatus") ){
                  firebaseStatus = FB_RUN;
                  led.slow();
                  continuousErroresReconnect=0;
                  Serial.print("ESPRUN;");
                }else{
                  instantFirebaseReconnection = millis();
                  continuousErroresReconnect++;
                  //debug.println("FAIL1");
                }
              }else{
                instantFirebaseReconnection = millis();
                continuousErroresReconnect++;
                //debug.println("FAIL0");
              }

              if ( continuousErroresReconnect>=3 ){
                ESP.reset();
              }
            }
          }break;
          
          case FB_RUN:{
            while ( Firebase.available() ){
              FirebaseObject event = Firebase.readEvent();
              eventType = event.getString("type");
              if ( eventType!="" ){
                eventData = event.getString("data");
                if ( eventData!="" ){
                  Serial.print(eventData+";");
                }
              }
            }
          }break;
        }
      }
    }break;
    default:{
      led.veryFast();
      if ( wifiStatusChanged ){
        wifiConnectTry=false;
      }else{
        if ( !wifiConnectTry ){
          wifiConnectTry = true;
          WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        }
      }
    }break;
  }
 
  while( Serial.available() ){
    commandPartial = String( (char)Serial.read() );
    //debug.println("["+String(c)+"]");
    if ( commandPartial=="\n" || commandPartial=="\r" ){
    }else{
      if ( commandPartial==";" ){
        newCommand=true;
      }else{
        command=command+commandPartial;
      }
    }

    if ( newCommand ){
      //debug.println("Escribiendo..." + command);
      Firebase.setString("/newStatus", command);
      if ( Firebase.success() ){
        commandSent = true;
      }else{
        String getted = Firebase.getString("/newStatus");
        if ( Firebase.success() ){
          if ( command == getted ){
            commandSent = true;
          }else{
            //debug.println("No son iguales");
            commandSent = false;
          }
        }else{
          //debug.println("No se puede leer");
          commandSent = false;
        }
      }

      if ( commandSent ){
        //debug.println("Todo ok...");
        Serial.print(command+"OK;");
        //debug.println(">>");
        continuousErroresData = 0;
      }else{
        //debug.println( "Error: " + Firebase.error());
        Serial.print(command+"FAIL;");
        //debug.println(">>");
        continuousErroresData++;        
      }

      newCommand = false;
      command = "";
      commandSent = false;
    }
  }

  if ( continuousErroresData>=3 ){
    firebaseStatus = FB_RECONNECT;
    instantFirebaseReconnection = millis() + lapseFirebaseReconnection;
  }
}
