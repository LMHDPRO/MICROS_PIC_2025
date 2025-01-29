#include <xc.h>

// Configuración del microcontrolador
#pragma config FOSC = INTOSCIO_EC
#pragma config WDT = OFF
#pragma config PBADEN = OFF
#pragma config LVP = OFF

#define _XTAL_FREQ 1000000

// Mapear dígitos a segmentos (a-g) para ánodo común
const unsigned char digitMap[10] = {
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111  // 9
};

// Variables para el estado de los botones
unsigned char lastStateRB0 = 1; // Estado anterior de RB0 (asume que los botones están en pull-up)
unsigned char lastStateRB1 = 1; // Estado anterior de RB1

void main(void) {
    // Configuración de puertos
    TRISD = 0x00; // PORTD como salida (segmentos a-g)
    TRISC = 0xF0; // RC0, RC1 como salidas (líneas de selección)
    TRISB = 0x03; // RB0 y RB1 como entradas (botones)
    PORTD = 0x00; // Limpiar segmentos
    PORTC = 0x00; // Limpiar líneas de selección
    PORTB = 0x00; // Limpiar PORTB

    unsigned char unidades = 0; // Contador de unidades
    unsigned char decenas = 0;  // Contador de decenas

    while (1) {
        // Mostrar UNIDADES
        PORTC = 0b00000001;         // Activar línea D2 (unidades)
        PORTD = digitMap[unidades]; // Enviar segmentos de unidades
        __delay_ms(5);              // Retardo de multiplexación

        // Apagar todos los segmentos antes de cambiar de dígito
        PORTD = 0x00;

        // Mostrar DECENAS
        PORTC = 0b00000010;         // Activar línea D3 (decenas)
        PORTD = digitMap[decenas];  // Enviar segmentos de decenas
        __delay_ms(5);              // Retardo de multiplexación

        // Apagar todos los segmentos antes de cambiar de dígito
        PORTD = 0x00;

        // Leer el estado de los botones
        unsigned char currentStateRB0 = RB0;
        unsigned char currentStateRB1 = RB1;

        // Detectar flanco de bajada en RB0 (botón de incremento)
        if (currentStateRB0 == 0 && lastStateRB0 == 1) {
            unidades++; // Incrementar unidades
            if (unidades > 9) {
                unidades = 0;
                decenas++;
                if (decenas > 9) {
                    decenas = 0; // Reiniciar a 0 después de 99
                }
            }
        }

        // Detectar flanco de bajada en RB1 (botón de decremento)
        if (currentStateRB1 == 0 && lastStateRB1 == 1) {
            if (unidades == 0) {
                unidades = 9;
                if (decenas == 0) {
                    decenas = 9; // Reiniciar a 99 después de 00
                } else {
                    decenas--;
                }
            } else {
                unidades--;
            }
        }

        // Actualizar el estado anterior de los botones
        lastStateRB0 = currentStateRB0;
        lastStateRB1 = currentStateRB1;

        // Pequeño retardo para evitar rebotes
        __delay_ms(50);
    }
}