const int sensorsEnable[8] = {22,24,26,28,30,32,34,36};
const int sensorsSignal[8] = {9,8,7,6,5,4,3,2};
String input="";

void setup() {
  // put your setup code here, to run once:
  for (int i=0; i<8; i++){
    pinMode(sensorsEnable[i],OUTPUT);
    digitalWrite(sensorsEnable[i],HIGH);

    pinMode(sensorsSignal[i],INPUT);
  }
  Serial.begin(115200);
  while (!Serial){
    
  }
  Serial.println("Detector de movimiento");
  Serial.println("-- Envie algo para comenzar --");
  while(!Serial.available()){
    
  }
  while(Serial.available()>0){
    char c = Serial.read(); 
  }

  for (int i=0 ;i<8; i++){
    digitalWrite(sensorsEnable[i],LOW);
  }
  input="";
}

void loop() {
  for (int i =0 ;i<8; i++){
    if ( digitalRead(sensorsSignal[i]) == LOW ){
      input+=" 1";
    }else{
      input+=" 0";
    }
  }
  Serial.println(input);
  input="";
}
