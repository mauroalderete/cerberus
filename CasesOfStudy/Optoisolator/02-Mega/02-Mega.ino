void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
}

String debug;

void loop() {

  if (Serial.available()) {
    int inByte = Serial.read();
    debug=String(char(inByte))+">>";
    Serial.println(debug);
    Serial1.write(inByte);
  }
  
  if (Serial1.available()) {
    int inByte = Serial1.read();
    debug = ">>"+String(char(inByte));
    Serial.println(debug);
  }

  
}
