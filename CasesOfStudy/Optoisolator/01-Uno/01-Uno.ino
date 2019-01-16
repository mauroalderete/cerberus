void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(7, INPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, digitalRead(7));
}
