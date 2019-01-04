# Cerberus

## Fuente de alimentación

La fuente de alimentacion por red de distribucipon electrica se divide en 3 etapas conectadas en serie:

- *Protección de linea principal* Posee un toma industrial, descarga a tierra, disyuntor y termica.
- *RemoteShutdown* Circuito desarrollado para conectarse de manera inalambrica y cortar a distancia el flujo electrico. Se reestablece tambien a distancia
- *SourceSelector* Prepara las tenciones adecuadas para que cada modulo de fuente pueda seleccionar el origen de la corriente electrica dependiendo de la presencia o no de corriente en la red.

# Componentes

## Red

- Enchufe 220V
- Térmica 10A
- Disyuntor

## Generator RedSignal

- Puente rectificador "Plano" **[Dimensiones]**
- Transformador 220V 8V/10V 1.5A **[Condensadores] (8V) ==> 1200+560; (10)==>2200**
- Condensador electrolitico:
    - 100uf 16V
    - 1200uf 25V **[Depende de transformador]**
    - 560uf 25V **[Depende de transformador]**
    - 2200uf 25V **[Depende de transformador]**
- Bornera 2 x 3
- LM7805
- Resistencia 220ohm 1/8W
- Diodo LED Rojo

## Remote Shutdown

- Rele doble de 5V para arduino

## Source Selector

- Puente rectificador "Plano" **[Dimensiones]**
- Condensador 22000uf 25V/63V **[Diametro]**
- Rele doble de 5V para arduino
- Transformador 220V a 12V 2<A<4
- Resistencia:
  - 22Kohm 1/8W
  - 15Kohm 1/8W
- Borneras 2 x 9
- Borneras 3 x 2

## Source Selector Regulator

- Bornera 2 x 2
- Capacitores Electroliticos
    - 100uf 25V x 2
- Resistencia 220ohm 1/8W
- LM7805
- Diodo LED Rojo

## Enable Device Source
***Requiere experimentar con diferentes valores de resistencias***

- Bornera 2 x 1
- Tira de pines [4]
- 4N25
- IRF520N
- Resistencias:
    - 1M 1/8 5% SMD 0805
    - 330 1/8 5% SMD 0805
    - 1K5 1/8 5% **SMD 0805** / 1K 1/4 5%

## Motherboard

- Bornera 2 x20
- Conector de Borde x5      ***AFECTA EL DISEÑO***
- Resistencia 1M 5% 1/8W SMD 0805

## Data Device Control

- BC548
- Resistencias:
    - 1k8 1/4
    - Tira de pines [4] x6
    - Conector [a DataDeviceHandler] [11]

## Device Data Handler PIR01

- Conector [a Data Device Control] [11]
- 4n35 DIP
- Diodos
    1n4148
- Resistencias
    - 1M 1/8W 5% SMD 0805
    - 560ohm 1/2W

## Device Data Handler INDOORSIREN01

- Conector [a Data Device Control] [11]
- 4n35 DIP
- Resistencias
    - 330 1/8W
    - 220 1/8W
    - 1M 1/8W 5% SMD 0805
- Mosfet IRF520N