#include "ErrorManager.h"

ErrorManager::ErrorManager(){
    errors[ARDUINO_RESET] = new Error(ARDUINO_RESET,"Arduino reiniciado");
    errors[ESP_RESET] = new Error(ESP_RESET,"ESP reiniciado");
    errors[UART_UNAVAILABLE] = new Error(UART_UNAVAILABLE,"Puerto serie no esta disponible");
    errors[UART_BUFFER] = new Error(UART_BUFFER,"Datos en el buffer de recepcion perdidos");
    errors[UART_TIMEOUT] = new Error(UART_TIMEOUT,"Tiempo de espera agotado");
    errors[UART_FAIL] = new Error(UART_FAIL,"Comando fallido");
    errors[UART_UNKNOWN] = new Error(UART_UNKNOWN,"Desconocido");
    
}

void ErrorManager::newError(unsigned int typeError){
    errors[typeError]->push();
}