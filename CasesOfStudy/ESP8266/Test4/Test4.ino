#include <ESP8266WiFi.h>

const char* ssid = "TeleCentro-3964";
const char* password = "271097533";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(750);
    Serial.println(".");
  }
}

void loop() {
  delay(2000);
  Serial.println("Hola");
}
