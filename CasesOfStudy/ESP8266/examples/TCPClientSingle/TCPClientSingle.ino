/**
 * @example TCPClientSingle.ino
 * @brief The TCPClientSingle demo of library WeeESP8266. 
 * @author Wu Pengfei<pengfei.wu@itead.cc> 
 * @date 2015.02
 * 
 * @par Copyright:
 * Copyright (c) 2015 ITEAD Intelligent Systems Co., Ltd. \n\n
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version. \n\n
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "ESP8266.h"

#define SSID        "TeleCentro-3964"
#define PASSWORD    "271097533"
#define HOST_NAME   "192.168.0.31"
#define HOST_PORT   (6363)

ESP8266 wifi(Serial1,115200);

void setup(void)
{
    Serial.begin(115200);
    Serial.print("setup begin\r\n");

    delay(1000);

    if(wifi.kick()){
      Serial.println("AT");
      Serial.println("+OK");
    }else{
      Serial.println("AT");
      Serial.println("ERROR");
    }
  
    Serial.print("FW Version:");
    Serial.println(wifi.getVersion().c_str());
      
    if (wifi.setOprToStationSoftAP()) {
        Serial.print("SUCCESS:to station ok\r\n");
    } else {
        Serial.print("ERROR:to station err\r\n");
    }
 
    if (wifi.joinAP(SSID, PASSWORD)) {
        Serial.print("Join AP success\r\n");
        Serial.print("IP:");
        Serial.println( wifi.getLocalIP().c_str());       
    } else {
        Serial.print("Join AP failure\r\n");
    }
    
    if (wifi.disableMUX()) {
        Serial.print("single ok\r\n");
    } else {
        Serial.print("single err\r\n");
    }
    
    Serial.print("setup end\r\n");
}
 
void loop(void)
{
    uint8_t buffer[128] = {0};
    
    if (wifi.createTCP(HOST_NAME, HOST_PORT)) {
        Serial.print("create tcp ok\r\n");

        char hello[] = "Hello, this is client!";
        if ( wifi.send((const uint8_t*)hello, strlen(hello)) ){
          Serial.println("SUCCESS: send");
        }else{
          Serial.println("ERROR: send");
        }
        
        uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);
        if (len > 0) {
            Serial.print("Received:[");
            for(uint32_t i = 0; i < len; i++) {
                Serial.print((char)buffer[i]);
            }
            Serial.print("]\r\n");
        }
        
    } else {
        Serial.print("create tcp err\r\n");
    }
    
    if (wifi.releaseTCP()) {
        Serial.print("release tcp ok\r\n");
    } else {
        Serial.print("release tcp err\r\n");
    }
    delay(10000);
}
     
