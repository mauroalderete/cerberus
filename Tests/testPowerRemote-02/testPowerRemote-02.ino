#define PWRFASE 50
#define PWRNEUTRO 51
#define PWRAVAILABLE 53

void setup() {

  pinMode(PWRFASE, OUTPUT);
  pinMode(PWRNEUTRO, OUTPUT);
  pinMode(PWRAVAILABLE, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(PWRFASE,LOW);
  digitalWrite(PWRNEUTRO,LOW);
  digitalWrite(LED_BUILTIN,LOW);

  for (int i=0;i<3;i++){
    digitalWrite(LED_BUILTIN,HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN,LOW);
    delay(500);
  }
}

void loop() {
  if ( digitalRead(PWRAVAILABLE) == HIGH ){
      digitalWrite(LED_BUILTIN,HIGH);
      digitalWrite(PWRFASE,HIGH);
      digitalWrite(PWRNEUTRO,HIGH);
  }else{
      digitalWrite(LED_BUILTIN,LOW);
      digitalWrite(PWRFASE,LOW);
      digitalWrite(PWRNEUTRO,LOW);
  }

}
