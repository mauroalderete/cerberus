#include <SoftwareSerial.h>

#define GSM_RX 8
#define GSM_TX 9

SoftwareSerial gsmSerial(GSM_RX, GSM_TX);
void setup() {

  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  Serial.println("GSM SIM800L V1.0 Comm");
  Serial.println("Iniciando...");

  gsmSerial.begin(9600);
  delay(100);
/*
  gsmSerial.println("AT"); //Once the handshake test is successful, it will back to OK
  delay(500);
  updateSerial();
  gsmSerial.println("AT+CSQ"); //Signal quality test, value range is 0-31 , 31 is the best
  delay(500);
  updateSerial();
  gsmSerial.println("AT+CCID"); //Read SIM information to confirm whether the SIM is plugged
  delay(500);
  updateSerial();
  gsmSerial.println("AT+CREG?"); //Check whether it has registered in the network
  delay(500);
*/
  Serial.println("GSM initialized");
}

void loop() {
  updateSerial();
}

void updateSerial(){
  if (Serial.available()){
    while (Serial.available()){
      gsmSerial.write(Serial.read());//Forward what Serial received to Software Serial Port
    }
  }

  if (gsmSerial.available()){
    while (gsmSerial.available()){
      Serial.write(gsmSerial.read());//Forward what Serial received to Software Serial Port
    }
  }
}
