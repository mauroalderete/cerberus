#include <ESP8266WiFi.h> 
#include <ArduinoJson.h>
#include <FirebaseArduino.h>


#define FIREBASE_HOST "cerberus-rayquendev.firebaseio.com"
#define FIREBASE_AUTH "MTxmcq7dCu9tGNuvAOGZqm9wOnpLXtbLMtXbUym9"
//#define WIFI_SSID "TeleCentro-3964"
//#define WIFI_PASSWORD "271097533"
#define WIFI_SSID "Cerberus"
#define WIFI_PASSWORD "tormentasolar"

int state = LOW;

void setup()
{
  Serial.begin(115200);
  pinMode (2, OUTPUT);
  digitalWrite (2, LOW);

  delay(100);
  Serial.print("Estado Inicial:");
  Serial.println(state);
  

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando WIFI");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500); 
  }
  Serial.println("OK");
  Serial.print("conectado:");
  Serial.println(WiFi.localIP ());
  Serial.println("Conectando Firebase");
  Firebase.begin (FIREBASE_HOST, FIREBASE_AUTH);
  delay(3000);
  Serial.println("Consultando Firebase...");

  Serial.print("Greeting... ");
  String greeting = Firebase.getString("/greeting");
  if ( Firebase.success() ){
    Serial.println(greeting);
  }else{
    if ( Firebase.failed() ){
      Serial.println("Error: "+Firebase.error());
    }else{
      Serial.println("Error: unknown");
    }
  }

  Serial.print("Estado... ");
  state = Firebase.getInt("/Estado");
  if ( Firebase.success() ){
    Serial.println(state);
  }else{
    if ( Firebase.failed() ){
      Serial.println("Error: "+Firebase.error());
    }else{
      Serial.println("Error: unknown");
    }
  }
  
  Serial.println("Preparando eventos");
  Firebase.stream("/Estado");
  Serial.print("Iniciando Stream to Estado ");
  if ( Firebase.success() ){
    Serial.println("Ok");
  }else{
    if ( Firebase.failed() ){
      Serial.println("Error: "+Firebase.error());
    }else{
      Serial.println("Error: unknown");
    }
  }
}

String eventType = "";
String eventPath = "";
String eventData = "";

int count = 0;

void loop () {
  if ( Firebase.available() ){
    count++;
    Serial.println("");
    Serial.println("["+String(count)+"]Nuevo evento detectado");
    delay(1);

    if ( Firebase.success() ){
      Serial.println("Available Ok");
    }else{
      Serial.println("Available fail");
    }
    if ( Firebase.failed() ){
      Serial.println("Error: "+Firebase.error());
    }
    else
    {
      Serial.println("Error: unknown");
    }
    delay(1);
    
    FirebaseObject event = Firebase.readEvent();

    Serial.println("WIFIStatus: " + String(WiFi.status()));

    Serial.print("  Tipo: ");
    eventType = event.getString("type");
    if ( Firebase.success() ){
      Serial.println(eventType);
    }else{
      if ( Firebase.failed() ){
        Serial.println("[ERROR] "+Firebase.error());
      }else{
        Serial.println("[ERROR] Unknown");
      }
    }

    if ( eventType == "put" ){
  
      Serial.print("  Ruta: ");
      eventPath = event.getString("path");
      if ( Firebase.success() ){
        Serial.println(eventPath);
      }else{
        if ( Firebase.failed() ){
          Serial.println("[ERROR] "+Firebase.error());
        }else{
          Serial.println("[ERROR] Unknown");
        }
      }
  
      Serial.print("  Data: ");
      eventData = event.getString("data");
      if ( Firebase.success() ){
        Serial.println(eventData);
      }else{
        if ( Firebase.failed() ){
          Serial.println("[ERROR] "+Firebase.error());
        }else{
          Serial.println("[ERROR] Unknown");
        }
      }
  
      Serial.print("Estado nuevo... ");
      state = Firebase.getInt("Estado");
      if ( Firebase.success() ){
        Serial.println(state);
      }else{
        if ( Firebase.failed() ){
          Serial.print("Error: "+Firebase.error());
        }else{
          Serial.print("Error: unknown");
        }
      }
    }
  }
}
