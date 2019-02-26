#define OFF String("OFF")
#define ON_ACTIVATED String("ON_ACTIVATED")
#define ON_DEACTIVATED String("ON_DEACTIVATED")

#define SERIAL_DEBUG  Serial
#define SERIAL_ESP Serial1

/*
 * Clase para sensores
 */

bool ping;

bool sendCommand(String data){
  SERIAL_DEBUG.println("sendingCommand");
  bool _timeout = false;
  bool _foundCommand = false;
  bool _error = true;
  char c;
  String _response="";

  SERIAL_ESP.print(data+";");
  SERIAL_DEBUG.println("");
  unsigned long _timeoutStartCount = millis();
  
  while ( !_timeout && !_foundCommand){
    while( SERIAL_ESP.available() ){
      c = (char)SERIAL_ESP.read();
      if ( c!='\n' && c!='\r' ){
        if (c == ';'){
          _foundCommand = true;
          SERIAL_DEBUG.println("recibido algo");
        }else{
          _response+=c;
        }
      }
    }
    
    if ( millis() - _timeoutStartCount > 4000 ){
      _timeout = true;
      SERIAL_DEBUG.println("timeout");
    }
  }
  
  if ( _foundCommand ){
    SERIAL_DEBUG.println("recibido: "+_response);
    if ( _response == data ){
      SERIAL_DEBUG.println("OK");
      _error = false;
    }else{
      SERIAL_DEBUG.println("FAIL");
    }
  }
  return !_error;
}

/*
 * CLASE para manejar los sensores
 */

class Sensor{
  private:
    int _input;           //pin en arduino de la señal del sensor
    int _enable;          //pin en arduino del control del sensor
    String _id;
    bool _currentSignal;  //ultima señal recibida
    bool _lastSignal;     //señal recibida en el ciclo anterior
    bool _signalChanged;          //indica cuando hubo un cambio en la señal recibida
    String _status;               //estado del sensor, debe coincidir con BD
    bool _notifyChange;
 
  public:
    Sensor(int input, int enable, String id){
      _input = input;
      _enable = enable;
      _id = id;
      _currentSignal = false;
      _lastSignal = false;
      _notifyChange = false;
      _status = OFF;
      pinMode(_input,INPUT);
      pinMode(_enable,OUTPUT);
      digitalWrite(_enable, HIGH);
    }
        
    bool changed(){
      return _signalChanged;
    }
    
    String status(){
      return _status;
    }

    void on(){
      if ( _currentSignal ){
        updateStatus(ON_ACTIVATED);
      }else{
        updateStatus(ON_DEACTIVATED);
      }      
    }

    void off(){
      updateStatus(OFF);
    }

    void updateStatus( String newStatus ){
      if ( newStatus==OFF ){
          digitalWrite( _enable, HIGH );
          _status = OFF;
      }
      if ( newStatus==ON_ACTIVATED ){
          digitalWrite( _enable, LOW );
          _status = ON_ACTIVATED;
      }
      if ( newStatus==ON_DEACTIVATED ){
          digitalWrite( _enable, LOW);
          _status = ON_DEACTIVATED;
      }
      setNotified();
    }

    void setNotified(){
      _notifyChange = true;
    }

    bool notifyChange(){
      return _notifyChange;
    }

    void changeNotified(){
      _notifyChange = false;
    }

    bool sendStatus(){
      if ( sendCommand("S"+_id+_status) ){
        changeNotified();
      }
    }      
    
    void Loop(){
      if ( _status==ON_ACTIVATED || _status==ON_DEACTIVATED ){
        _lastSignal = _currentSignal;
        if ( digitalRead(_input)==HIGH ){
            _currentSignal = false;
        }else{
            _currentSignal = true;
        }
        
        if ( _lastSignal != _currentSignal ){
            _signalChanged = true;
        }else{
            _signalChanged = false;
        }

        if ( _signalChanged ){
            if ( _currentSignal ){
                updateStatus(ON_ACTIVATED);
            }else{
                updateStatus(ON_DEACTIVATED);
            }
        }
      }
    }
};

#define SENSORS 8
#define ACTUATORS 3
Sensor *Sensors[8];
//Actuator *Actuators[3];

void setup() {
  int s=0;
  int a=0;

  ping=false;

  Sensors[0] = new Sensor(9,22,"0");
  Sensors[1] = new Sensor(8,24,"1");
  Sensors[2] = new Sensor(7,26,"2");
  Sensors[3] = new Sensor(6,28,"3");
  Sensors[4] = new Sensor(5,30,"4");
  Sensors[5] = new Sensor(4,32,"5");
  Sensors[6] = new Sensor(3,34,"6");
  Sensors[7] = new Sensor(2,36,"7");

//  Actuators[0] = new Actuator(33,0);
//  Actuators[1] = new Actuator(35,1);
//  Actuators[2] = new Actuator(37,2);

//  for (a=0;a<ACTUATORS;a++){
//    Actuators[0]->prepare();
//  }

  SERIAL_ESP.begin(115200);
  SERIAL_DEBUG.begin(115200);
  randomSeed(analogRead(0));
  delay(100);

  SERIAL_DEBUG.println(">>ARDRST");
}

void loop() {
  int s=0;
  int a=0;

  /*
   * REBISO SI HAY CAMBIOS EN LOS SENSORES
   */
  bool actuatorsTurnOn = false;
  for (s=0; s<SENSORS; s++){
      Sensors[s]->Loop();
      if ( Sensors[s]->changed() && Sensors[s]->status()==ON_ACTIVATED ){
        actuatorsTurnOn = true;
      }
  }

/*
  if ( actuatorsTurnOn ){
    for (a=0;a<ACTUATORS;a++){
      Actuators[a]->updateStatus(ON_ACTIVATED);
    }
  }
*/

  /*
   * ENVIO LAS NOTIFICACIONES
   */
  for (s=0; s<SENSORS; s++){
    if ( Sensors[s]->notifyChange() ){
      Sensors[s]->sendStatus();
    }
  }

  if ( ping ){
    SERIAL_ESP.print( "PING"+String( random(0,1000))+";" );
    ping = false;
  }
  
  char c;
  bool reciveCommand = false;
  String command="";
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

    if ( reciveCommand && command=="S0ON"){ Sensors[0]->on(); reciveCommand=false; }
    if ( reciveCommand && command=="S0OFF"){ Sensors[0]->off(); reciveCommand=false; }
    if ( reciveCommand && command=="S1ON"){ Sensors[1]->on(); reciveCommand=false; }
    if ( reciveCommand && command=="S1OFF"){ Sensors[1]->off(); reciveCommand=false; }
    if ( reciveCommand && command=="S2ON"){ Sensors[2]->on(); reciveCommand=false; }
    if ( reciveCommand && command=="S2OFF"){ Sensors[2]->off(); reciveCommand=false; }
    if ( reciveCommand && command=="S3ON"){ Sensors[3]->on(); reciveCommand=false; }
    if ( reciveCommand && command=="S3OFF"){ Sensors[3]->off(); reciveCommand=false; }
    if ( reciveCommand && command=="S4ON"){ Sensors[4]->on(); reciveCommand=false; }
    if ( reciveCommand && command=="S4OFF"){ Sensors[4]->off(); reciveCommand=false; }
    if ( reciveCommand && command=="S5ON"){ Sensors[5]->on(); reciveCommand=false; }
    if ( reciveCommand && command=="S5OFF"){ Sensors[5]->off(); reciveCommand=false; }
    if ( reciveCommand && command=="S6ON"){ Sensors[6]->on(); reciveCommand=false; }
    if ( reciveCommand && command=="S6OFF"){ Sensors[6]->off(); reciveCommand=false; }
    if ( reciveCommand && command=="S7ON"){ Sensors[7]->on(); reciveCommand=false; }
    if ( reciveCommand && command=="S7OFF"){ Sensors[7]->off(); reciveCommand=false; }
    if ( reciveCommand && command=="A0ON"){ Sensors[0]->on(); reciveCommand=false; }
    if ( reciveCommand && command=="A0OFF"){ Sensors[0]->off(); reciveCommand=false; }
    if ( reciveCommand && command=="A1ON"){ Sensors[1]->on(); reciveCommand=false; }
    if ( reciveCommand && command=="A1OFF"){ Sensors[1]->off(); reciveCommand=false; }
    if ( reciveCommand && command=="A2ON"){ Sensors[2]->on(); reciveCommand=false; }
    if ( reciveCommand && command=="A2OFF"){ Sensors[2]->off(); reciveCommand=false; }
    if ( reciveCommand && command=="PING"){ ping=true; reciveCommand=false; }
    if ( reciveCommand && command=="ESPRST"){ ping=false; reciveCommand=false; }
  }
}
