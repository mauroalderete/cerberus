#include "Arduino.h"
#include "Error.h"

Error::Error(int code, String description){
    timer = millis() - ERROR_INTERVAL;
    this->code = code;
    this->description = description;
}

void Error::push(){
    if ( millis() - timer > ERROR_INTERVAL ){
        char c = EEPROM.read(code);
        if (c<255){
            c++;
            EEPROM.write(code,c);
        }
    }
}

unsigned int Error::getQuantity(){
    return EEPROM.read(code);
}

void Error::clearCount(){
    EEPROM.write(code,0);
}