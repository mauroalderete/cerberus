#include <EEPROM.h>

byte out;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  for (int i=2;i<=10;i++){
    pinMode(i, OUTPUT);
    digitalWrite(i,HIGH);    
  }

  out = 0;
  Serial.begin(9600);
  delay(10);

  byte value = EEPROM.read(0);
  value++;
  EEPROM.write(0,value);
  Serial.println("START!! antes se iniciaron "+String(value-1)+" veces, ahora suman "+String(value)+" veces");
}

String debug;
String binario;

void loop() {

  if (out<10){
    debug=">>   ";
  }else{
    if (out<100){
      debug=">>  ";
    }else{
      debug=">> ";
    }
  }
  debug+=String(out)+":";
  binario="";
  for (int i=2; i<=10; i++){
    if ( ( out & round(pow(2,(i-2))) )>0 ){
      digitalWrite(i,LOW);
      binario=String("1")+binario;
    }else{
      digitalWrite(i,HIGH);
      binario=String("0")+binario;
    }
  }
  //Serial.println(debug+binario);
  delay(500);
  out++;
}

void serialEvent(){
  char c;
  while(Serial.available()){
    c = (char)Serial.read();
  }
  if (c == 'c'){
    byte value = EEPROM.read(0);
    Serial.println("    "+String(value-1)+" reinicios hasta ahora");
  }
  if (c == 'r'){
    EEPROM.write(0,0);
    Serial.println("    Cuenta reiniciada");
  }
}
