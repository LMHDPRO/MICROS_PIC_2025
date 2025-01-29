/*
 * File:   PWM_main.c
 * Author: IGNITER
 *
 * Created on 29 de enero de 2025, 10:07 AM
 */


#include <xc.h>

#include <xc.h>

// Configuración del microcontrolador
#pragma config FOSC = INTOSCIO_EC
#pragma config WDT = OFF
#pragma config PBADEN = OFF
#pragma config LVP = OFF

#define _XTAL_FREQ 1000000

void PWM_Init(void);
void Set_PWM_DutyCycle(unsigned int duty);

void main(void) {
    unsigned char duty = 0; // Duty cycle inicial (0%)
    char direction = 1;     // Dirección del cambio (1 = subir, -1 = bajar)

    // Configurar PWM
    PWM_Init();
    // Configurar PORTD como salida para los segmentos
    TRISD = 0x00; // PORTD como salida
    PORTD = 0x3F; // Mostrar '0' en display ánodo común

    // Configurar RC0 como salida para el transistor NPN
    TRISCbits.TRISC0 = 0; // RC0 como salida
    LATCbits.LATC0 = 1;   // Activar transistor (Encender display)

    while (1) {
        Set_PWM_DutyCycle(duty); // Ajustar brillo del LED
        
        __delay_ms(200); // Retardo de 200ms

        // Cambiar duty cycle en un 10% en la dirección actual
        duty += (10 * direction);

        // Cambiar dirección al llegar a 0% o 100%
        if (duty >= 100) {
            direction = -1;
            duty = 100;
        } else if (duty <= 0) {
            direction = 1;
            duty = 0;
        }
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

