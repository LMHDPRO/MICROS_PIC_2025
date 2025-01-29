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

// Variables para el contador
unsigned char unidades = 0;
unsigned char decenas = 0;
unsigned char centenas = 0;
unsigned char millares = 0;

// Variable para controlar el dígito actual
unsigned char currentDigit = 0;

void main(void) {
    // Configuración de puertos
    TRISD = 0x00; // PORTD como salida (segmentos a-g)
    TRISC = 0x00; // PORTC como salida (líneas de selección D1-D4)
    PORTD = 0x00; // Limpiar segmentos
    PORTC = 0x00; // Limpiar líneas de selección

    while (1) {
        // Multiplexación de los dígitos usando switch-case
        switch (currentDigit) {
            case 0:
                // Mostrar UNIDADES (D1)
                PORTC = 0b00000001;         // Activar D1
                PORTD = digitMap[unidades]; // Enviar segmentos de unidades
                break;

            case 1:
                // Mostrar DECENAS (D2)
                PORTC = 0b00000010;         // Activar D2
                PORTD = digitMap[decenas];  // Enviar segmentos de decenas
                break;

            case 2:
                // Mostrar CENTENAS (D3)
                PORTC = 0b00000100;         // Activar D3
                PORTD = digitMap[centenas]; // Enviar segmentos de centenas
                break;

            case 3:
                // Mostrar MILLARES (D4)
                PORTC = 0b00001000;         // Activar D4
                PORTD = digitMap[millares]; // Enviar segmentos de millares
                break;
        }

        // Retardo de multiplexación
        __delay_ms(5);

        // Apagar todos los segmentos antes de cambiar de dígito
        PORTD = 0x00;

        // Cambiar al siguiente dígito
        currentDigit++;
        if (currentDigit > 3) {
            currentDigit = 0; // Reiniciar al primer dígito (D1)
        }

        // Incrementar el contador (ejemplo)
        unidades++;
        if (unidades > 9) {
            unidades = 0;
            decenas++;
            if (decenas > 9) {
                decenas = 0;
                centenas++;
                if (centenas > 9) {
                    centenas = 0;
                    millares++;
                    if (millares > 9) {
                        millares = 0; // Reiniciar después de 9999
                    }
                }
            }
        }

        // Retardo para que el contador aumente más lentamente
        __delay_ms(500);
    }
}