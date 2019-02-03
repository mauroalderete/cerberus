#ifndef ErrorManager_h
#define ErrorManager_h
 
#include "Arduino.h"
#include "Error.h"

#define ARDUINO_RESET 0
#define ESP_RESET 1
#define UART_UNAVAILABLE 2
#define UART_BUFFER 3
#define UART_TIMEOUT 4
#define UART_FAIL 5
#define UART_UNKNOWN 6

class ErrorManager{
    Error *errors[7];

    public:

    ErrorManager();

    void newError(unsigned int typeError);

};

#endif