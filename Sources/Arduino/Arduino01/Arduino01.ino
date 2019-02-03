#include <ErrorManager.h>

#define ON_ACTIVATED String("ON_ACTIVATED")
#define ON_DEACTIVATED String("ON_DEACTIVATED")
#define OFF String("OFF")

#define TURN_ON String("TURN_ON")
#define TURN_OFF String("TURN_OFF")

class Messenger{
  private:
    HardwareSerial *_port;
    bool _timeout;
    unsigned long _timeoutLimit;
    String _recived;
    bool _error;
    unsigned int _errorCode;
    String _errorDescription;
    bool _warning;
    unsigned int _warningCode;
    String _warningDescription;
    ErrorManager *_em;

  public:
  
    Messenger(){
      _timeoutLimit = (unsigned long)(2000);
      _timeout = false;
      _recived = "";
    }

    void setPort(HardwareSerial *port){
      _port = port;
    }

    void setTimeout(unsigned long timeout){
      _timeoutLimit = timeout;
    }

    void setErrorManager(ErrorManager *em){
      _em = em;
    }

    bool send(String msg){

      _timeout = false;
      _error = false;
      
      if ( _port ){

        char temp;
        while( _port->available() ){
            temp = _port->read();
            _error = true;
            _em->newError(UART_BUFFER);
        }
        
        _port->println(msg);

        unsigned long _timeoutStartCount = millis();
        unsigned long currentTimeoutCount;
        _timeout = false;
        while( !_port->available() ){
          currentTimeoutCount = millis();
          if ( currentTimeoutCount - _timeoutStartCount > _timeoutLimit ){
            _timeout = true;
            break;
          }
        }

        if ( _timeout ){
          _em->newError(UART_TIMEOUT);
          _error = true;
        }else{
          char c;
          _recived="";
          while( _port->available() ){
            c = _port->read();
            if (c == ';'){
              break;
            }else{
              _recived+=c;
            }
          }

          if ( _recived == "OK" ){
          }else{
            _error = true;
            if ( _recived == "FAIL" ){
              _em->newError(UART_FAIL);
            }else{
              _em->newError(UART_UNKNOWN);
            }
          }
        }
      }else{
        _error = true;
        _em->newError(UART_UNAVAILABLE);
      }

      return !_error;
    }

    void Loop(){
      //ESP a Arduino
      //registrar error de coneccion wifi
      //registrar reinicio de esp
      //registrar error de coneccion con firebase
      
      //solicitar reporte de errores (R=0,1,0,255)
      /*
       * Solicitar reporte de errores
       * >>R=0,1,0,255
       * Cada parametro corresponde a reinicios y codigos de errores, se cuentan la cantidad de errores que hubieron
       * Cada codigo de error tiene un temporizador que solo incrementa el registro cada x tiempo
       * Por lo que el valor del error es una muestra de la cantidad de tiempo, acumulado del error hasta el momento de la peticion de reporte
       * [0]=Reinicios de arduino
       * [1]=Reinicios de esp
       * [2]=Errores de UART
       * [3]=Errores de wifi
       * [3]=Errores de Firebase
       * Los errores solo son limpiados de manera manual
       */

      //setaer nuevo estado de sensor
      //setear nuevo estado de actuador

      
    }

    bool sendSensorStatus(int id, String status){
      return send("S="+String(id)+","+status+";");
    }

    bool sendActuatorStatus(int id, String status){
      return send("A="+String(id)+","+status+";");      
    }

    bool sendRestartDetected(int quantity){
      return send("RD="+String( quantity )+";");
    }
};

class Sensor{
  private:
    int _input;           //pin en arduino de la señal del sensor
    int _enable;          //pin en arduino del control del sensor
    bool _currentSignal;  //ultima señal recibida
    bool _lastSignal;     //señal recibida en el ciclo anterior
    bool _signalChanged;  //indica cuando hubo un cambio en la señal recibida
    String _status;       //estado del sensor, debe coincidir con BD
    Messenger *_messenger;

    unsigned int _id;
    bool _changeNotificated;
 
  public:
    Sensor(int input, int enable, unsigned int id){
      _input = input;
      _enable = enable;
      _currentSignal = false;
      _lastSignal = false;
      _status = OFF;
      _id = id;
    }
    
    void prepare(){
      pinMode(_input,INPUT);
      pinMode(_enable,OUTPUT);
      _status = OFF;
    }

    void setMessenger(Messenger *msn){
      _messenger = msn;
    }

    bool changed(){
      return _signalChanged;
    }

    String status(){
      return _status;
    }

    String updateStatus(String newStatus){
      if ( _status == newStatus ){
        if ( !_messenger->sendSensorStatus(_id,"INDF") ){
        }
      }

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
      if ( !_messenger->sendSensorStatus(_id,_status) ){
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

class Actuator{
  private:
    int _enable;
    String _status;
    Messenger *_messenger;

    unsigned int _id;
  
  public:
    Actuator(int enable, unsigned int id){
      _enable = enable;
      _id=id;
      _status = OFF;
    }
    
    void prepare(){
      pinMode(_enable,OUTPUT);
      _status = OFF;
    }

    void setMessenger(Messenger *msn){
      _messenger = msn;
    }

    void updateStatus(String newStatus){
      if ( _status == newStatus ){
        if ( !_messenger->sendActuatorStatus(_id,"INDF") ){
        }
      }

      if ( newStatus==OFF ){
        digitalWrite( _enable, HIGH );
        _status = OFF;
      }
      if ( newStatus==ON_ACTIVATED ){
        digitalWrite( _enable, LOW );
        _status = ON_ACTIVATED;
      }
      
      if ( !_messenger->sendActuatorStatus(_id,_status) ){

      }
    }
};

#define SENSORS 8
#define ACTUATORS 3
Sensor *Sensors[8];
Actuator *Actuators[3];
Messenger messenger;

void setup() {
  int s=0;
  int a=0;
  
  Sensors[0] = new Sensor(9,22,0);
  Sensors[1] = new Sensor(8,24,1);
  Sensors[2] = new Sensor(7,26,2);
  Sensors[3] = new Sensor(6,28,3);
  Sensors[4] = new Sensor(5,30,4);
  Sensors[5] = new Sensor(4,32,5);
  Sensors[6] = new Sensor(3,34,6);
  Sensors[7] = new Sensor(2,36,7);

  for (s=0;s<SENSORS;s++){
    Sensors[s]->prepare();
  }

  Actuators[0] = new Actuator(33,0);
  Actuators[1] = new Actuator(35,1);
  Actuators[2] = new Actuator(37,2);
  
  for (a=0;a<ACTUATORS;a++){
    Actuators[0]->prepare();
  }

  Serial.begin(115200);

  delay(100);

  messenger.setPort(&Serial);
  
  for (s=0;s<SENSORS;s++){
    Sensors[s]->setMessenger(&messenger);
  }
  for (a=0;a<ACTUATORS;a++){
    Actuators[a]->setMessenger(&messenger);
  }

}

void loop() {
  int s=0;
  int a=0;
  bool actuatorsTurnOn = false;
  for (s=0; s<SENSORS; s++){
      Sensors[s]->Loop();
      if ( Sensors[s]->changed() && Sensors[s]->status()==ON_ACTIVATED ){
        actuatorsTurnOn = true;
      }
  }

  if ( actuatorsTurnOn ){
    for (a=0;a<ACTUATORS;a++){
      Actuators[a]->updateStatus(ON_ACTIVATED);
    }
  }
}
