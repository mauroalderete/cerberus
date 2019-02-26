#define PWRFASE 50
#define PWRNEUTRO 51
#define PWRAVAILABLE 53

unsigned int previousPowerAvailable;

#define OFF String("OFF")
#define ON String("ON")
#define ON_ACTIVATED String("ON_ACTIVATED")
#define ON_DEACTIVATED String("ON_DEACTIVATED")

#define SERIAL_DEBUG Serial
#define SERIAL_ESP Serial1

#define IS_SENSOR true
#define IS_ACTUATOR false

bool ACTIVATE_ALARM = false;

#define INTERVAL_NOTIFIED 4000

String data;

bool reset;
unsigned long resetLapse;
#define RESET_INTERVAL 5000
#define IGNORE_INTERVAL 60000

class Device{
  public:
  bool sensor;
  int signalPin;
  int enablePin;
  String id;
  bool currentSignal;
  bool lastSignal;
  bool signalChanged;
  String stat;
  bool notified;
  unsigned long lapse;
  bool ignore;
  unsigned long ignoreLapse;
    
  Device(bool _type, String _id, int _enablePin, int _signalPin=-1){
    sensor = _type;
    signalPin = _signalPin;
    enablePin = _enablePin;
    id=_id;
    currentSignal = false;
    lastSignal = false;
    signalChanged = false;
    updateStat(OFF);
    notified = false;
    lapse = 0;
    ignore=false;
    ignoreLapse=0;

    if ( sensor ) pinMode(signalPin,INPUT);
    pinMode(enablePin,OUTPUT);
    digitalWrite(enablePin, HIGH);
  }

  void updateStat(String _stat){
    notified = true;

    if (_stat==OFF){
      digitalWrite(enablePin, HIGH);
      stat = OFF;
    }
    
    if (_stat==ON){
      digitalWrite(enablePin, LOW);
      if ( sensor ){
        if ( stat==OFF ){
          SERIAL_DEBUG.println("Encendiendo Sensor... Esperando...");
          stat = ON_DEACTIVATED;
          ignore=true;
          ignoreLapse = millis();
        }else{
          if ( currentSignal ){
            SERIAL_DEBUG.println("ALARMA 1");
            ACTIVATE_ALARM = true;
            stat = ON_ACTIVATED;
          }else{
            stat = ON_DEACTIVATED;
          }
        }
        //}
        //se supone que recien se arranco el sensor, porlo que probable que no se sense adecuadamente
        //se inicia un conteo para ignorar cualquier comportamieto del sensor durante unos 2 minutos despues de iniciado
        //esto dara tiempo al pir para estabilizarce.

      }else
      {
        stat = ON;
      }
    }

    if (_stat==ON_ACTIVATED){
      SERIAL_DEBUG.println("ALARMA 2");
      ACTIVATE_ALARM = true;
      digitalWrite(enablePin, LOW);
      stat = ON_ACTIVATED;
    }

    if (_stat==ON_DEACTIVATED){
      digitalWrite(enablePin, LOW);
      stat = ON_DEACTIVATED;
    }
  }

  void Loop(){
    if ( sensor ){

      if ( ignore ){
        if ( millis() - ignoreLapse > IGNORE_INTERVAL ){
          ignore=false;
          SERIAL_DEBUG.println("Sensor encendido previamente ya esta listo");
        }
      }
      
      if ( stat != OFF && ignore==false ){
        lastSignal = currentSignal;
        if ( digitalRead(signalPin)==HIGH ){
          currentSignal = false;
        }else{
          currentSignal = true;
        }

        if ( lastSignal != currentSignal ){
          if ( currentSignal ){
            updateStat(ON_ACTIVATED);
          }else{
            updateStat(ON_DEACTIVATED);
          }
        }
      }
    }else{
      if ( stat==OFF ){
        if ( ACTIVATE_ALARM ){
          updateStat(ON);
        }
      }
    }
  }
};

bool ping;
String command="";
bool reciveCommand = false;

#define DEVICES 11
Device *Devices[DEVICES];

bool cambio;
unsigned long lapseCambio;

void setup() {
  reset=true;
  resetLapse = millis();

  ping=false;
  cambio=false;
  lapseCambio = 0;

  pinMode(PWRFASE, OUTPUT);
  pinMode(PWRNEUTRO, OUTPUT);
  pinMode(PWRAVAILABLE, INPUT);
  previousPowerAvailable=LOW;

  Devices[0] = new Device(IS_SENSOR,"0",22,9);
  Devices[1] = new Device(IS_SENSOR,"1",24,8);
  Devices[2] = new Device(IS_SENSOR,"2",26,7);
  Devices[3] = new Device(IS_SENSOR,"3",28,6);
  Devices[4] = new Device(IS_SENSOR,"4",30,5);
  Devices[5] = new Device(IS_SENSOR,"5",32,4);
  Devices[6] = new Device(IS_SENSOR,"6",34,3);
  Devices[7] = new Device(IS_SENSOR,"7",36,2);

  Devices[8] = new Device(IS_ACTUATOR,"0",33);
  Devices[9] = new Device(IS_ACTUATOR,"1",35);
  Devices[10] = new Device(IS_ACTUATOR,"2",37);

  SERIAL_ESP.begin(115200);
  SERIAL_DEBUG.begin(115200);
  randomSeed(analogRead(0));
  delay(100);

  SERIAL_DEBUG.println(">>ARDRST");
}

void loop() {
  int d=0;

  data = "";

  for ( d=0; d<DEVICES; d++ ){
    Devices[d]->Loop();

    
    if ( Devices[d]->notified ){
      cambio = true;
    }
    
    if ( Devices[d]->sensor ){
      if ( Devices[d]->stat == OFF ){
        data +="F";
      }else{
        if ( Devices[d]->stat == ON_ACTIVATED ){
          data +="A";
        }else{
          data +="D";
        }
      }
    }else{
      if (Devices[d]->stat == ON){
        data +="N";
      }else{
        data +="F";
      }
    }
  }

  if ( reset ){
    if ( millis() - resetLapse > RESET_INTERVAL){
      reset=false;
      if ( digitalRead(PWRAVAILABLE)==HIGH ){
        digitalWrite(PWRFASE,HIGH);
        digitalWrite(PWRNEUTRO,HIGH);
        SERIAL_DEBUG.println("Conectando a energia electrica");
      }
    }
  }

  if ( digitalRead(PWRAVAILABLE)==HIGH ){
    data+="P";
  }else{
    data+="B";
  }
  if ( digitalRead(PWRAVAILABLE) != previousPowerAvailable ){
    cambio = true;
  }
  previousPowerAvailable = digitalRead(PWRAVAILABLE);

  ACTIVATE_ALARM = false;

  if ( cambio ){
    //comienzo a notificar
    if ( millis() - lapseCambio > 5000 ){
      SERIAL_ESP.print(data+";");
      SERIAL_DEBUG.println(data+";");
      lapseCambio = millis();
    }
  }
  
  char c;
  while ( SERIAL_ESP.available() ){
    c = (char)SERIAL_ESP.read();
    if ( c!='\n' && c!='\r' ){
      if ( c==';' ){
        reciveCommand = true;
        SERIAL_DEBUG.println("Comando Recibido: "+command);
      }else{
        command = command + c;
      }
    }

    if ( reciveCommand ){

      if ( command=="PING" ){
        ping=true;
      }

      if ( command=="PWRON" ){
        digitalWrite(PWRFASE,HIGH);
        digitalWrite(PWRNEUTRO,HIGH);
        SERIAL_DEBUG.println("Ejecutando PWRON");
      }

      if ( command=="PWROFF" ){
        digitalWrite(PWRFASE,LOW);
        digitalWrite(PWRNEUTRO,LOW);
        SERIAL_DEBUG.println("Ejecutando PWROFF");
      }

      if ( command=="ESPRST" ){
        ping = false;
        cambio = true;
        SERIAL_DEBUG.println("Detectado reinicio de ESP");
        SERIAL_DEBUG.println("Actualizando datos");
      }

      if ( command=="OK" ){
        for ( d=0; d<DEVICES; d++){
          Devices[d]->notified = false;
        }
        cambio=false;
        SERIAL_DEBUG.println("Confirmacion recibida");
      }

      if ( command=="S0OFF") { Devices[0]->updateStat(OFF); }
      if ( command=="S0ON") { Devices[0]->updateStat(ON); }
      if ( command=="S1OFF") { Devices[1]->updateStat(OFF); }
      if ( command=="S1ON") { Devices[1]->updateStat(ON); }
      if ( command=="S2OFF") { Devices[2]->updateStat(OFF); }
      if ( command=="S2ON") { Devices[2]->updateStat(ON); }
      if ( command=="S3OFF") { Devices[3]->updateStat(OFF); }
      if ( command=="S3ON") { Devices[3]->updateStat(ON); }
      if ( command=="S4OFF") { Devices[4]->updateStat(OFF); }
      if ( command=="S4ON") { Devices[4]->updateStat(ON); }
      if ( command=="S5OFF") { Devices[5]->updateStat(OFF); }
      if ( command=="S5ON") { Devices[5]->updateStat(ON); }
      if ( command=="S6OFF") { Devices[6]->updateStat(OFF); }
      if ( command=="S6ON") { Devices[6]->updateStat(ON); }
      if ( command=="S7OFF") { Devices[7]->updateStat(OFF); }
      if ( command=="S7ON") { Devices[7]->updateStat(ON); }
      if ( command=="A0OFF") { Devices[8]->updateStat(OFF); }
      if ( command=="A0ON") { Devices[8]->updateStat(ON); }
      if ( command=="A1OFF") { Devices[9]->updateStat(OFF); }
      if ( command=="A1ON") { Devices[9]->updateStat(ON); }
      if ( command=="A2OFF") { Devices[10]->updateStat(OFF); }
      if ( command=="A2ON") { Devices[10]->updateStat(ON); }
      reciveCommand = false;
      command = "";
    }

  }

  if ( ping ){
    SERIAL_DEBUG.println("Enviando PING");
    SERIAL_ESP.print( "PING"+String( random(0,1000))+";" );
    ping = false;
  }
}
