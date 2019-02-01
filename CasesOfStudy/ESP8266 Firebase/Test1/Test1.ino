#include <ESP8266WiFi.h> 
#include <ArduinoJson.h>
#include <FirebaseArduino.h>


#define FIREBASE_HOST "cerberus-rayquendev.firebaseio.com"
#define FIREBASE_AUTH "MTxmcq7dCu9tGNuvAOGZqm9wOnpLXtbLMtXbUym9"
#define WIFI_SSID "TeleCentro-3964"
#define WIFI_PASSWORD "271097533"

int state = LOW;

void setup()
{
  Serial.begin(115200);
  pinMode (LED_BUILTIN, OUTPUT);
  digitalWrite (LED_BUILTIN, state);

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
  Firebase.setInt("Estado",1);
}

void loop () { 
  String greeting = Firebase.getString("greeting");
  Serial.print("recived: "+greeting);
  state = Firebase.getInt("Estado");
  Serial.print(" - State:");
  Serial.println(state);
  digitalWrite (LED_BUILTIN,state);
  delay(10000);
}
