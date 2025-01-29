PROGRAMACIÓN ESTRUCTURADA CON MICROCONTROLADORES

MICROCONTROLADOR: PIC18F4550

PICKIT 3 

UTILIZANDO EL SIMULADOR PICSIMLAB EN LA MODALIDAD DE BREADBOARD CON EL MICROCONTROLADOR PIC18F4550

COMPILADOR XC8 CON MPLAB X

ESTO CUENTA CON LAS SIGUIENTES PRÁCTICAS.

Práctica 1
Realiza los efectos de LEDs en PIC18F4550.
Programa los siguientes efectos LED en C
Utiliza ciclos for, delays, puertos y las estructuras de programación que necesites.

Utiliza un botón para cambiar de effecto.

1. Despazamiento Básico
  Solo un LED se enciende y los demás se apagan. Después de un tiempo se apaga el LED y se enciende el siguiente.
2. Desplazamiento con Rebote
  Efecto tipo ping-pong o también conocido como el efecto del "auto fantástico". Similar al desplazamiento básico, pero al llegar al final cambia la dirección del desplazamiento.
3. Parpadeo (Blink)
  Todos los LEDs se encienden y apagan al mismo tiempo.
4. Alternado
  Se encienden un LED sí y otro no, luego se alterna. El efecto hace parecer que las luces bailan.
5. Medidor de Intensidad
  Enciende el siguiente LED sin apagar el anterior. Luego apaga los LEDs uno por uno, empezando por el último que se encendió.
6. Medidor de Intensidad (v2)
  Similar al medidor de intensidad, pero empezando por los extremos y terminando en el centro.
7. Viborita
  Similar al juego de Snake. Solo 4 LEDs consecutivos se encienden. Conforme avanza la viborita se enciende el siguiente LED pero se apaga el último de la cola.
8. Contador Binario
  Contador en Binario de 0 a 255.


Práctica 2
 Ejercicio 1
Encender un LED de manera que aumente su intensidad un 10% cada 200ms, al llegar a 100% decrementa 10% cada 200ms.
Utilizar la señal para activar un display de 7 segmentos con un transistor NPN. El display muestra el número 0.

Ejercicio 2.
Incrementar o decrementar la intensidad un 10% utilizando 2 push buttons. (Uno para incrementar y otro para decrementar).
La señal activa un LED o un Display.

Ejercicio 3. 
Realizar un contador de 0 a 99 con dos displays de 7 segmentos conectados al mismo puerto.
El incremento/decremento se realiza con 2 push buttons.
Usar variables para llevar el conteo de las unidades y las decenas.
Usa la estructura switch-case.
