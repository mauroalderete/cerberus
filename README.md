# Cerberus

## Fuente de alimentación

La fuente de alimentacion por red de distribucipon electrica se divide en 3 etapas conectadas en serie:

- *Protección de linea principal* Posee un toma industrial, descarga a tierra, disyuntor y termica.
- *RemoteShutdown* Circuito desarrollado para conectarse de manera inalambrica y cortar a distancia el flujo electrico. Se reestablece tambien a distancia
- *SourceSelector* Prepara las tenciones adecuadas para que cada modulo de fuente pueda seleccionar el origen de la corriente electrica dependiendo de la presencia o no de corriente en la red.

# Componentes

Conector de Borde SYC:
- 2x18
- 2x25
- 2x31

*Caracteristicas*
- Cuerpo PBT + fibra de vidrio (UL-94V-O)
- Contacto bronce fosforoso - oro selectivo
- Corriente 3A
- Resistencia de contacto 30mOHM

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

## Enable Device Source
***Requiere experimentar con diferentes valores de resistencias***

- Bornera 2 x 1
- Tira de pines [5]
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