#include <Arduino_Actuator.h>
#include <Arduino_ErrorManager.h>
#include <Arduino_Messenger.h>
#include <Arduino_Sensor.h>
#include <Cerberus.h>


#define SENSORS 8
#define ACTUATORS 3
Sensor *Sensors[8];
Actuator *Actuators[3];
Messenger messenger;
ErrorManager errorManager;

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
    Sensors[s]->setErrorManager(&errorManager);
  }
  for (a=0;a<ACTUATORS;a++){
    Actuators[a]->setMessenger(&messenger);
    Actuators[a]->setErrorManager(&errorManager);
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
/*
  if ( errorManager.errorsAvailable() ){
    if ( !errorManager.getNotified() ){
      if ( messenger.sendNotifiedErrorAvailable() ){
        errorManager.setNotifiedSuccessful();
        messenger.sendActivationNotNotified();
      }
    }
  }
*/
  
}
