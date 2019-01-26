#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <Hash.h>

#define SCREEN_WIDTH 80
int screenWidthCount = 0;

const char* ssid = "TeleCentro-3964";
const char* password = "271097533";

const int led = 2;
bool ledStatus=true;

WebSocketsClient webSocket;

bool disconect = false;


/*
 * EVENTO PARA WEBSOCKET CLIENT
 */

void webSocketEvent(WStype_t type, uint8_t * payload, size_t lenght) {
  Serial.println("Evento detectado");
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      disconect = true;
    break;
    case WStype_CONNECTED:
    {
      Serial.printf("[WSc] Connected to url: %s\n",  payload);
      webSocket.sendTXT("Buenas!!");
    }
    break;
    case WStype_TEXT:
      Serial.printf("[WSc] get text: %s\n", payload);
    break;
    case WStype_BIN:
      Serial.printf("[WSc] get binary lenght: %u\n", lenght);
      hexdump(payload, lenght);
      // send data to server
      // webSocket.sendBIN(payload, lenght);
    break;
  }
}

/*
 * SETUP
 */


void setup() {
  pinMode(2, OUTPUT);
  
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  while (!Serial){
    
  }

  for(uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }
    
  Serial.println("Conectando a Punto de Acceso");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(2, ledStatus?HIGH:LOW);
    ledStatus=ledStatus?false:true;
    screenWidthCount++;
    if (screenWidthCount>SCREEN_WIDTH){
      Serial.print("\n\r");
      screenWidthCount=0;
    }
  }

  Serial.println("Ok");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Conectando a Websocket en Servidor");

  webSocket.begin("192.168.0.31", 6363);
  webSocket.onEvent(webSocketEvent);
}

/*
 * MAIN
 */

void loop() {
  //delay(2000);
  //digitalWrite(2, ledStatus?HIGH:LOW);
  //ledStatus=ledStatus?false:true;
    webSocket.loop();
}
