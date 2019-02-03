/*
  Error.h - Libreria para manejar un error.
  Creado por Mauro J. E. Alderete, Febreso 2, 2019.
*/
#ifndef Error_h
#define Error_h
 
#include "Arduino.h"
#include <EEPROM.h>
#define ERROR_INTERVAL 600000
 
class Error
{
    private:
    unsigned int code;
    String description;
    unsigned long timer;

  public:
    Error(int code, String description);

    void push();
    unsigned int getQuantity();
    void clearCount();
};
 
#endif