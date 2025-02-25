/*
 * File:   CARGA7SEGmain.c
 * Author: m8
 *
 * Created on 06 February 2025, 13:17
 */

#include <xc.h>
#include <stdio.h>

#pragma config FOSC = INTOSCIO_EC
#pragma config WDT = OFF
#pragma config PBADEN = OFF 
#pragma config LVP = OFF
//#pragma config WDTE = OFF

#define _XTAL_FREQ 8000000
#define IN 1
#define OUT 0
#define ON 1
#define OFF 0
#define Boton PORTBbits.RB0
#define Boton2 PORTBbits.RB1
#define D1 LATCbits.LC1
#define D2 LATCbits.LC0
#define D4 LATCbits.LC6
#define D3 LATCbits.LC7
#define dot LATDbits.LD7
#define LED_P1 LATBbits.LB2
#define LED_P2 LATBbits.LB3

void check_button(void);
void retardo(int us);
void contar_up();
void fin();
void mux(void);
void update_leds(void);
void inicio();
void verificar_fin_juego();
void AcelerarJuego();

//Para el PIC FÍSICO
//unsigned char val7[] =  {0x3F, 0x30, 0x57, 0x4F, 0x69, 0x6E, 0x7E, 0x0B, 0x7F, 0x6F};
                        //0     1     2       3   4       5       6   7       8   9
//Para el PIC Virtual
unsigned char val7[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

int turno = 0;
unsigned vel = 5000;
short int dir = 0;
short int u1 = 5;
short int u2 = 5;
short int d1 = 0;
short int d2 = 0;
short int juego_terminado = 0; // 0: Juego en curso, 1: Juego terminado
short int ganador = 0; // 0: Sin ganador, 1: Jugador 1, 2: Jugador 2

void main(void) {
    OSCCONbits.IRCF = 0b111; // Oscilador interno 8MHz
    TRISD = OUT;
    TRISBbits.RB0 = IN;
    TRISBbits.RB1 = IN;
    TRISBbits.RB2 = OUT;
    TRISBbits.RB3 = OUT;
    TRISC = 0;
    LATD = val7[0];
    while (1) {
        inicio();
        check_button();
        contar_up();
        fin();
    }
    return;
}

void retardo(int us){
    if (us >= 1000){
        int milis = (us-us%1000)/1000;
        for (int i =0; i < milis; i++){
            __delay_ms(1);
        }
        for (int i =0; i < us%1000; i+=100){
            __delay_us(100);
        }  
    }
    return;
}

void update_leds() {
    if (dir == 1) {
        LED_P1 = ON;
        LED_P2 = OFF;
    } else if (dir == 2) {
        LED_P1 = OFF;
        LED_P2 = ON;
    }
}

void mux() {
    LATD = val7[u1] | 0x80; // Activa el punto decimal
    D4 = ON;
    retardo(vel);
    D4 = OFF;
    LATD = val7[d1];
    D3 = ON;
    retardo(vel);
    D3 = OFF;
    LATD = val7[u2] | 0x80;
    D2 = ON;
    retardo(vel);
    D2 = OFF;
    LATD = val7[d2];
    D1 = ON;
    retardo(vel);
    D1 = OFF;
    update_leds();
}

void check_button() {
    if (Boton == ON) {
        turno++;
        dir = 1;
        __delay_ms(1);
        dir = 1;
        AcelerarJuego();
    }
    if (Boton2 == ON) {
        turno++;
        dir = 2;
        __delay_ms(1);
        dir = 2;
        AcelerarJuego();
    }
}

void contar_up() {
    while (!juego_terminado) { // Mientras el juego no haya terminado
        check_button(); // Verifica si se ha presionado un botón
        mux(); // Actualiza los displays

        switch (dir) {
            case 1: // Incrementar d1 y decrementar d2
                d1++;
                if (d1 > 9) {
                    d1 = 0;
                    u1++;
                    if (u1 > 9) {
                        u1 = 0;
                        verificar_fin_juego();
                    }
                }

                d2--;
                if (d2 < 0) {
                    d2 = 9;
                    u2--;
                    if (u2 < 0) {
                        u2 = 9;
                    }
                }
                break;

            case 2: // Incrementar d2 y decrementar d1
                d2++;
                if (d2 > 9) {
                    d2 = 0;
                    u2++;
                    if (u2 > 9) {
                        u2 = 0;
                        verificar_fin_juego();
                    }
                }

                d1--;
                if (d1 < 0) {
                    d1 = 9;
                    u1--;
                    if (u1 < 0) {
                        u1 = 9;
                    }
                }
                break;
        }

        verificar_fin_juego(); // Verifica si el juego ha terminado
    }
}

void fin() {
    if (juego_terminado) {
        while (1) {
            // Mostrar el ganador
            if (ganador == 1) {
                // Jugador 1 gana
                u1 = 1;
                u2 = 1;
                d1 = 0;
                d2 = 0;
            } else if (ganador == 2) {
                // Jugador 2 gana
                u1 = 2;
                u2 = 2;
                d1 = 0;
                d2 = 0;
            }
            mux(); // Actualizar displays
        }
    }
}

void inicio(){
    while (turno == 0){
        check_button(); 
        mux();
    }
}

void verificar_fin_juego() {
    if (u1 == 0) {
        juego_terminado = 1; // Juego terminado
        ganador = 2; // Jugador 2 gana
    } 
    else if (u2 == 0) {
        juego_terminado = 1; // Juego terminado
        ganador = 1; // Jugador 1 gana
    }
}

void AcelerarJuego(){
    if (turno % 4 == 0){
            vel = vel * 0.95;
        }
}
