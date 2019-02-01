const int sensors[8] = {22,24,26,28,30,32,34,36};
int count=0;

void setup() {
  // put your setup code here, to run once:
  for (int i=0; i<8; i++){
    pinMode(sensors[i],OUTPUT);
  }
  Serial.begin(115200);
  while (!Serial){
    
  }
  Serial.println("Iniciando secuencia - envie una letra para continuar");
  while(!Serial.available()){
    
  }
  
}

void loop() {
  // put your main code here, to run repeatedly:
  for (int i =0 ;i<8; i++){
    if (count==i){
      digitalWrite(sensors[i],LOW);
    }else{
      digitalWrite(sensors[i],HIGH);
    }
  }  
  Serial.println(String("encendiendo: ")+count);
  delay(2000);
  count++;
  if (count>=8){
    count=0;
  }
  

}
