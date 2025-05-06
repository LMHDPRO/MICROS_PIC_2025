/*
 * File:   MainBT.c
 * Author: José Eduardo Pardinaz Mercado
 *
 * Created on 6 de mayo de 2025, 01:01 AM
 * Happy Birthday Diana!
 */

#include <xc.h>
#define _XTAL_FREQ 8000000

// CONFIGURATION
#pragma config PLLDIV = 1
#pragma config CPUDIV = OSC1_PLL2
#pragma config USBDIV = 2
#pragma config FOSC = INTOSCIO_EC
#pragma config FCMEN = OFF
#pragma config IESO = OFF
#pragma config PWRT = OFF
#pragma config BOR = OFF
#pragma config WDT = OFF
#pragma config PBADEN = OFF
#pragma config LVP = OFF
#pragma config MCLRE = ON

// Definiciones de control de motor
#define IN1 LATBbits.LATB0
#define IN2 LATBbits.LATB1
#define IN3 LATBbits.LATB2
#define IN4 LATBbits.LATB3

// Definiciones de funciones
void UART_Init(void);
char UART_Read(void);
void UART_ClearBuffer();
char UART_Read_Timeout(int timeout_ms);
void Stop();
void Forward();
void Backward();
void Left();
void Right();

void main(void) {
    TRISB = 0x00;  // Puerto B como salida
    LATB = 0x00;
    OSCCONbits.IRCF = 7; // Config a 8MHz
    UART_Init();
    Stop();

    while (1) {
        char comando = UART_Read();
        switch (comando) {
            case 'F': Forward(); break;
            case 'B': Backward(); break;
            case 'L': Left(); break;
            case 'R': Right(); break;
            case 'S': Stop(); break;
            default: Stop(); break; // Opcional: detener si el comando es inválido
        }
    }
}

void UART_Init(void) {
    TRISCbits.TRISC7 = 1; // RX input
    TRISCbits.TRISC6 = 0; // TX output

    SPBRG = 51; // Baud rate ~9600 bps con 8 MHz
    TXSTAbits.BRGH = 1;   // High speed
    TXSTAbits.SYNC = 0;   // Async
    RCSTAbits.SPEN = 1;   // Enable serial port
    RCSTAbits.CREN = 1;   // Enable reception
    TXSTAbits.TXEN = 1;   // Enable transmission
    UART_ClearBuffer();
}

char UART_Read(void) {
    while (!PIR1bits.RCIF); // Esperar a que llegue dato
    return RCREG;           // Devolver dato recibido
}

void UART_ClearBuffer() {
    while(PIR1bits.RCIF) {
        char dummy = RCREG; // Lee y descarta datos residuales
        PIR1bits.RCIF = 0;
    }
}  

char UART_Read_Timeout(int timeout_ms) {
    while (!PIR1bits.RCIF && timeout_ms-- > 0) __delay_ms(1);
    return (timeout_ms > 0) ? RCREG : '\0'; // Retorna '\0' si hay timeout
}

void Stop() {
    IN1 = 0; IN2 = 0;
    IN3 = 0; IN4 = 0;
}

void Forward() {
    IN1 = 1; IN2 = 0;
    IN3 = 1; IN4 = 0;
}

void Backward() {
    IN1 = 0; IN2 = 1;
    IN3 = 0; IN4 = 1;
}

void Left() {
    IN1 = 0; IN2 = 1;
    IN3 = 1; IN4 = 0;
}

void Right() {
    IN1 = 1; IN2 = 0;
    IN3 = 0; IN4 = 1;
}

