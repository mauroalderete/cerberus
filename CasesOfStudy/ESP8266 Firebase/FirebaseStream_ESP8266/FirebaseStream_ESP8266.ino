//
// Copyright 2015 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// FirebaseStream_ESP8266 is a sample that stream bitcoin price from a
// public Firebase and optionally display them on a OLED i2c screen.

#include <FirebaseArduino.h>
#include <ESP8266WiFi.h>

// Set these to run example.
#define WIFI_SSID "TeleCentro-3964"
#define WIFI_PASSWORD "271097533"

void setup() {
  Serial.begin(115200);

  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  
  Firebase.begin("publicdata-cryptocurrency.firebaseio.com");
  Firebase.stream("/bitcoin/last");  
}

int countMax = 0;
int countSection = 0;


void loop() {
  if (Firebase.failed()) {
    Serial.println("streaming error");
    Serial.println(Firebase.error());
  }

  if ( WiFi.isConnected()){
    countSection=0;
    while(Firebase.available()) {
       countMax++;
       countSection++;
       FirebaseObject event = Firebase.readEvent();
       String eventType = event.getString("type");
       eventType.toLowerCase();

       Serial.println("["+String(countMax)+"]["+String(countSection)+"]");
       Serial.print("event: ");
       Serial.println(eventType);
       if (eventType == "put") {
         Serial.print("path: ");
         Serial.println(event.getString("path"));
         Serial.print("data: ");
         Serial.println(event.getString("data"));
       }
    }
  }else{
    Serial.println("[ERROR] Wifi");
  }
}
