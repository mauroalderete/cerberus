#define PWRFASE 50
#define PWRNEUTRO 51
#define PWRAVAILABLE 53

void setup() {

  pinMode(PWRFASE, OUTPUT);
  pinMode(PWRNEUTRO, OUTPUT);
  pinMode(PWRAVAILABLE, INPUT);

  digitalWrite(PWRFASE,LOW);
  digitalWrite(PWRNEUTRO,LOW);
  
  Serial.begin(115200);
  while (!Serial){
    
  }
  Serial.println("Control de energia remota");

  if ( digitalRead(PWRAVAILABLE)==HIGH ){
    Serial.println("SI hay corriente en linea");
  }else{
    Serial.println("NO hay corriente en linea");
  }
  
  Serial.println("--- envie algo para continuar ---");
  
  while(!Serial.available()){
    
  }
  while(Serial.available()>0){
    char c = Serial.read(); 
  }

  Serial.println("Corroborando estado de la linea");
  if ( digitalRead(PWRAVAILABLE)==HIGH ){
    Serial.println("SI hay corriente en linea");
    Serial.println("Iniciando encendido sostenido de la linea");
    digitalWrite(PWRFASE,HIGH);
    digitalWrite(PWRNEUTRO,HIGH);
  }else{
    Serial.println("NO hay corriente en linea");
    Serial.println("NO es posible conectarse a la red electrica");
  }
  
}

void loop() {
  if (Serial.available()>0){
    char c = Serial.read(); 
    if ( c=='d' ){
      Serial.println("Desconectando sistema de la red electrica...");
      delay(100);
      Serial.println("Bye");
      delay(300);
      digitalWrite(PWRFASE,LOW);
      digitalWrite(PWRNEUTRO,LOW);
    }
  }

}
