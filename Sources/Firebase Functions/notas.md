#Consideraciones de la base de datos

##Se generan los nodos:

- sensors
- actuators
- power
- system
- events

###sensors

Los nodos hijos corresponden con el id de cada sensor.

Cada sensor contendra información sobre su estado, su descripción, ubicación fisica y conexión electrica con el arduino.

El evento principal que dispara una alerta por correo electronico se da en el nodo de estado.

Una forma de manejar los estados propuesta al inicio era dividir las situaciones en 3 flags booleanos:

- **available**, indicaba si se encontraba electricamente conectado al circuito. Lo que significa que se puede comensar a utilizar.
- **enable**, indicaba si el sensor esta encendido o apagado. Setear este elemento debe ser util para controlar la cantidad de energía electrica consumida desde la bateria.
- **actived**, en caso de que los anteriores flags se encontraran en true, este valor indicaria si el sensor detecto algo o no.

Sin embargo esta propuesta tiene problemas de redundancia y de combinaciones no posibles. Por lo que se planea implementar un solo nodo de estado con multiples valores posibles.

- status

valores posibles:

- **DISCONNECTED**: desconectado electricamente
- **ON_DEACTIVATED**:    el sensor esta encendido, y por lo tanto conectado, pero no registra ningun movimiento.
- **ON_ACTIVATED**: el sensor esta encendido, por lo tanto conectado, y registra movimiento.
- **OFF**: el sensor esta conectado pero apagado.

Estos estados son escritos por ESP y leidos por los clientes.
Por el contrario los clientes pueden enviar un estado de preferencia atraves del nodo **preferredStatus**, con los siguientes valores:

- **DISCONNECT**: desconectar
- **TURN_ON**: encender
- **TURN_OFF**: apagar

Cuando un cliente escribe y cambia el estado de preferencia del sensor, ESP captura el evento y notifica al arduino que debe modificar el sensor al estado indicado. Esto provoca a su vez un cambio desde el ESP a Firebase y por tanto a los clientes quienes actualizan sus interfaces, confirmando de esta forma que el cambio fue producido.

###actuators

De manera similar al control de estados que se produce con los sensores, los actuadores poseen dos nodos.

- **status**, con los valores:
    - **DISCONNECTED**, indica que esta electricamente desconectado
    - **OFF**, indica que el actuador esta apagado
    - **ON**, indica que el actuador esta encendido y ejecutando su sonido o luz.
- **preferredStatus**:
    - **DISCONNECT**, se debe desconectar
    - **TURN_OFF**, se debe apagar
    - **TURN_ON**, se debe encender

###power

status:
- **AVAILABLE_DISCONNECTED**, funciona a bateria a pesar de que hay electricidad en la red.
- **AVAILABLE_CONNECTED**, funciona con la red electrica y las baterias se estan cargando
- **UNAVAILABLE**, no hay corriente en la red electrica, funciona a bateria

*cuando la luz se corta, el estado pasa a ser de AVAILABLE_CONNECTED a UNAVAILABLE. En este momento por seguridad Cerberus desconectara los terminales que controlan el flujo de la red electrica, para evitar continuos encendidos y apagados. Al mismo tiempo Firebase Function informara a los clientes de que no hay luz.*
*Cuando la luz regresa, y suponiendo que las baterias funcionan en condiciones, Cerberus informara del cambio de estado de UNAVAILABLE a AVAILABLE_DISCONNECTED, de esta forma los clientes podran decidir en desean conectar nuevamente enviando un CONNECTED a preferredStatus*
*Por performance, Cerberus una vez que recibe un nuevo estado en preferredStatus, coloca el estado a "WAITING" para que luego cuando otro usuario ejecute un nuevo estado, este estado pase a activar un evento de cambio. Este cambio que realiza Cerberus siembre activa un nuevo estado, pero es ignorado en el proceso de corroboracion de estados posibles*

preferredStatus:
- **CONNECTED**
- **DISCONNECTED**
- **WAITING**

###system

"system": {
    "success": false,
    "error": "",
    "ping": 0,
    "pong": 0,
    "lifeSignal": 0,
    "reconnections": 0,
    "restart": 0,
    "newReset": 0
}

success|boolean: responde si la ultima operacion fue realizada con exito o no
error|string: describe el error producido por la ultima orden ejecutada
ping|integer: es el numero aleatorio que las apps envian a Cerberus para identificar si suige con vida
pong|integer: si Cerberus sigue con vida, envia el numero recibido en ping a travez de este campo
lifeSignal|integer: campo que al modificar su valor permite enviar un mensaje como señal de vida del sistema
reconnections|integer: cantidad de reconexiones wifi que cerberus tuvo que realizar desde la ultima vez que envio una señal de vida.
restart|integer: cantidad de reinicios que cerberus sufrio desde la ultima vez que envio una señal de vida.
newReset|integer: numero aleatorio que activa un evento para notificar que el sistema se acaba de reiniciar