#define OFF String("OFF")
#define ON String("ON")
#define ON_ACTIVATED String("ON_ACTIVATED")
#define ON_DEACTIVATED String("ON_DEACTIVATED")

#define SERIAL_DEBUG  Serial
#define SERIAL_ESP Serial1

#define IS_SENSOR true
#define IS_ACTUATOR false

bool ACTIVATE_ALARM = false;

#define INTERVAL_NOTIFIED 4000

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
    
  Device(bool _type, String _id, int _enablePin, int _signalPin=-1){
    sensor = _type;
    signalPin = _signalPin;
    enablePin = _enablePin;
    id=_id;
    currentSignal = false;
    lastSignal = false;
    signalChanged = false;
    stat = OFF;
    notified = false;
    lapse = 0;

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
        if ( currentSignal ){
          ACTIVATE_ALARM = true;
          stat = ON_ACTIVATED;
        }else{
          stat = ON_DEACTIVATED;
        }
      }else
      {
        stat = ON;
      }
    }

    if (_stat==ON_ACTIVATED){
      ACTIVATE_ALARM = true;
      digitalWrite(enablePin, LOW);
      stat = ON_ACTIVATED;
    }

    if (_stat==ON_DEACTIVATED){
      digitalWrite(enablePin, LOW);
      stat = ON_DEACTIVATED;
    }

    //SERIAL_DEBUG.print("updateStat: ");
    //SERIAL_DEBUG.print(sensor?"S":"A");
    //SERIAL_DEBUG.println(id+stat);
  }

  void Loop(){
    if ( sensor ){
      if ( stat != OFF ){
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

  void sendStatus(){
    if ( notified ){
      if ( millis()-lapse > INTERVAL_NOTIFIED ){
        SERIAL_DEBUG.print("sendStatus: ");
        if ( sensor ){
          SERIAL_DEBUG.println("S"+id+stat+";");
          SERIAL_ESP.print("S"+id+stat+";");
        }else{
          SERIAL_DEBUG.println("A"+id+stat+";");
          SERIAL_ESP.print("A"+id+stat+";");
        }
        lapse = millis();
      }
    }
  }

  void analyzeResponse(String _response){
    if ( notified ){
      SERIAL_DEBUG.print("analyzeResponse:");
      if ( sensor ){
        if ( _response=="S"+id+stat+"OK" ){
          SERIAL_DEBUG.println("S"+id+" OK");
          notified = false;
        }else{
          SERIAL_DEBUG.println("S"+id+" FAIL");
        }
        
      }else{
        if ( _response=="A"+id+stat+"OK" ){
          SERIAL_DEBUG.println("A"+id+" OK");
          notified = false;
        }else{
          SERIAL_DEBUG.println("A"+id+" FAIL");
        }
      }
    }
  }

  bool analyzeCommand(String _command){
    if ( sensor ){
      if ( _command=="S"+id+OFF ){
        SERIAL_DEBUG.println("analyzeCommand S"+id+ " OFF Accept");
        updateStat(OFF);
        return true;
      }
      if ( _command=="S"+id+ON ){
        SERIAL_DEBUG.println("analyzeCommand S"+id+ " ON Accept");
        updateStat(ON);
        return true;
      }
    }else{
      if ( _command=="A"+id+OFF ){
        SERIAL_DEBUG.println("analyzeCommand A"+id+ " OFF Accept");
        updateStat(OFF);
        return true;
      }
      if ( _command=="A"+id+ON ){
        SERIAL_DEBUG.println("analyzeCommand A"+id+ " ON Accept");
        updateStat(ON);
        return true;
      }
    }
  }
};

bool ping;
String command="";
bool reciveCommand = false;

#define DEVICES 11
Device *Devices[DEVICES];

void setup() {
  ping=false;

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

  for ( d=0; d<DEVICES; d++ ){
    Devices[d]->Loop();
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

    if (reciveCommand && command=="PING"){ ping=true; reciveCommand=false; command = ""; }
    if (reciveCommand && command=="ESPRST"){ ping=false; reciveCommand=false; command = ""; }

    if ( reciveCommand ){
      for (d=0; d<DEVICES; d++){
        Devices[d]->analyzeResponse(command);
        Devices[d]->analyzeCommand(command);
      }
      reciveCommand = false;
      command = "";
    }
  }

  if ( ping ){
    SERIAL_ESP.print( "PING"+String( random(0,1000))+";" );
    ping = false;
  }

  for (d=0; d<DEVICES; d++){
    Devices[d]->sendStatus();
  }
}
