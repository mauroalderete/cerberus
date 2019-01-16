
void setup() {
  pinMode(LED_BUILTIN,OUTPUT);
  pinMode(8,INPUT);
  pinMode(6,INPUT);
  Serial.begin(9600);
}

String debug;

void loop() {

  if ( digitalRead(8)==HIGH ){
    debug = "4:HIGH ";
  }else{
    debug = "4:LOW ";
  }

  if ( digitalRead(6)==HIGH ){
    debug = debug + "1:HIGH ";
  }else{
    debug = debug + "1:LOW ";
  }
  Serial.println(debug);
  delay(3000);

}
