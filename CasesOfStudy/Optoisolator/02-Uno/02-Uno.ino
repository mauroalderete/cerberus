
void setup() {
  pinMode(LED_BUILTIN,OUTPUT);
  Serial.begin(9600);
}

void loop() {

  if (Serial.available()) {
    int inByte = Serial.read();

    if (inByte==97){
      digitalWrite(LED_BUILTIN,HIGH);
    }
    if (inByte==98){
      digitalWrite(LED_BUILTIN,LOW);
    }
    
    Serial.write(inByte+1);
  }
}
