void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(7, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(7, HIGH);
  delay(2000);
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(7, LOW);
  delay(2000);
}
