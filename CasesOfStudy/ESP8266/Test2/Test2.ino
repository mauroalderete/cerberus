#include <ESP8266WiFi.h>

#define SCREEN_WIDTH 80
int screenWidthCount = 0;

const char* ssid = "TeleCentro-3964";
const char* password = "271097533";

const int led = 2;
bool ledStatus=true;

void setup() {
  pinMode(2, OUTPUT);
  Serial.begin(74880);
  //Serial.setDebugOutput(true);
  Serial.println("Conecting");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(2, ledStatus?HIGH:LOW);
    ledStatus=ledStatus?false:true;
    screenWidthCount++;
    if (screenWidthCount>SCREEN_WIDTH){
      Serial.print("\n\r");
      screenWidthCount=0;
    }
  }

  Serial.println("Ok");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  delay(2000);
  digitalWrite(2, ledStatus?HIGH:LOW);
  ledStatus=ledStatus?false:true;
}
