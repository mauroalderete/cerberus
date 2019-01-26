#define CTRL_Z 26

unsigned long timer=0;
unsigned long current=0;

const long interval = 10000;

#define BAUDIOS 74880

void setup() {
  Serial.begin(BAUDIOS);   //coneccion con Terminal
  while (!Serial){
    ;
  }
  Serial.println("Start!!");
  Serial.println("Starting GSMPort...");
  Serial1.begin(BAUDIOS);  //coneccion con GSM
  while(!Serial1){
    ;
  }
  Serial.println("GSMPort Start!!");
  delay(3000);
  Serial.println("Ready!!!");
}

void loop() {
  if (Serial.available()) {
    int inByte = Serial.read();
    if (char(inByte)=='*'){
      inByte = CTRL_Z;
    }
    Serial1.write(inByte);
  }
  
  if (Serial1.available()) {
    int inByte = Serial1.read();
    Serial.write(inByte);
  }
  current=millis();
/*
  if ( current-timer>interval ){
    Serial1.println("AT+CREG?");
    timer=current;
  }
  */
}
