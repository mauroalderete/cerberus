void setup() {
  Serial.begin(9600);
  delay(100);
}

String input;

void loop() {
  input="";
  input+=digitalRead(2)==HIGH?"1 ":"0 ";
  input+=digitalRead(3)==HIGH?"1 ":"0 ";
  input+=digitalRead(4)==HIGH?"1 ":"0 ";
  input+=digitalRead(5)==HIGH?"1 ":"0 ";
  input+=digitalRead(6)==HIGH?"1 ":"0 ";
  input+=digitalRead(7)==HIGH?"1 ":"0 ";
  input+=digitalRead(8)==HIGH?"1 ":"0 ";
  input+=digitalRead(9)==HIGH?"1 ":"0 ";
  Serial.println(input);
  delay(50);
}
