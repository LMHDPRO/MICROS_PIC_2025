/*
 * File:   PWM_main.c
 * Author: IGNITER
 *
 * Created on 29 de enero de 2025, 10:07 AM
 */

#include <xc.h>

// Configuración del microcontrolador
#pragma config FOSC = INTOSCIO_EC
#pragma config WDT = OFF
#pragma config PBADEN = OFF
#pragma config LVP = OFF

#define _XTAL_FREQ 1000000 // Frecuencia del sistema (1 MHz)

void PWM_Init(void);
void Set_PWM_DutyCycle(unsigned int duty);

unsigned char duty = 50; // Duty cycle inicial (50%)

void main(void) {
    // Inicializar PWM
    PWM_Init();

    // Configurar botones en RB0 y RB1 como entradas
    TRISBbits.TRISB0 = 1; // Botón para aumentar
    TRISBbits.TRISB1 = 1; // Botón para disminuir
    INTCON2bits.RBPU = 0; // Habilitar resistencias pull-up internas

    while (1) {
        // Si se presiona RB0, aumentar brillo
        if (!PORTBbits.RB0) { 
            if (duty < 100) {
                duty += 10;
            }
            __delay_ms(200); // Esperar 200 ms
        }

        // Si se presiona RB1, disminuir brillo
        if (!PORTBbits.RB1) { 
            if (duty > 0) {
                duty -= 10;
            }
            __delay_ms(200); // Esperar 200 ms
        }

        Set_PWM_DutyCycle(duty); // Ajustar brillo del LED
    }
}

// Inicializar el PWM en CCP1
void PWM_Init(void) {
    TRISCbits.TRISC2 = 0; // RC2 como salida (CCP1)

    PR2 = 255; // Periodo del PWM
    CCP1CON = 0b00001100; // CCP1 en modo PWM
    CCPR1L = 0; // Duty cycle inicial en 0%
    
    // Configurar Timer2 para PWM
    T2CON = 0b00000100; // Prescaler 1:1, Timer2 ON
}

// Ajustar el duty cycle del PWM
void Set_PWM_DutyCycle(unsigned int duty) {
    unsigned int pwmValue = (duty * 255) / 100; // Convertir porcentaje a 8 bits (0-255)
    CCPR1L = pwmValue >> 2; // Parte alta en CCPR1L
    CCP1CONbits.DC1B = pwmValue & 0x03; // Bits bajos en CCP1CON
}
