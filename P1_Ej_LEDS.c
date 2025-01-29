/*
 * File:   Ej_LEDS.c
 * Author: José Pardiñaz
 *
 * Created on 21 de enero de 2025, 12:11 PM
 */

//PIC18F4550 Configuration Bit Settings
#pragma config FOSC = INTOSCIO_EC
#pragma config WDT = OFF
#pragma config PBADEN = OFF
#pragma config LVP = OFF

#define _XTAL_FREQ 1000000
#define IN 1
#define OUT 0
#define Boton PORTBbits.RB0
#define ON 1
#define OFF 0

#include <xc.h>
#include <math.h>

void efecto1(void);
void efecto2(void);
void efecto3(void);
void efecto4(void);
void efecto5(void);
void efecto6(void);
void efecto7(void);
void efecto7B(void);
void efecto8(void);
void CheckButton(void);

int efectoActual = 0;

 void main(void) {
    //Se declara un puerto de entrada y salida
    TRISD = OUT;
    TRISBbits.RB0 = IN;
    //Se crea un ciclo
    while(1){
        switch(efectoActual){
            case 0:
                __delay_ms(250);
                CheckButton();
                break;
            case 1:
                efecto1();
                break;
            case 2:
                efecto2();
                break;
            case 3:
                efecto3();
                break;
            case 4:
                efecto4();
                break;
            case 5:
                efecto5();
                break;
            case 6:
                efecto6();
                break;
            case 7:
                efecto7B();
                break;
            case 8:
                efecto8();
                break;
            default:
                efectoActual = 0;
                __delay_ms(500);
                break;
            
        }
    //Modifica el puerto
        //efecto1();
        //efecto2();
        //efecto3();
        //efecto4();
        //efecto5();
        //efecto6();
        //efecto7B();
        //{efecto8();
    }
    
    return;
}

    void efecto1(){ //Pin
        for(int i = 128; i >= 1; i/=2){
            LATD = i;
            __delay_ms(500);
            CheckButton();
            if(efectoActual !=1){
                break;
            }
        } 
}

void efecto2(){ //Pinpon
    for(int i = 128; i >= 1; i/=2){
            LATD = i;
            __delay_ms(500);
            CheckButton();
            if(efectoActual !=2){
                break;
            }
        }
        for(int i = 2; i <= 65; i*=2){
            LATD = i;
            __delay_ms(500);
            CheckButton();
            if(efectoActual !=2){
                break;
            }
        }
}

void efecto3(){ //Blink172
            LATD = 0xFF;
            __delay_ms(200);
            CheckButton();
            LATD = 0x00;
            __delay_ms(200);
}

void efecto4(){
        LATD = 0xAA;
        __delay_ms(500);
        CheckButton();
        LATD = 0x55;
        __delay_ms(500);
}

void efecto5(){
 for(int i = 0; i <= 255; i = i*2+1){
            LATD = i;
            __delay_ms(500);
            CheckButton();
            if(efectoActual !=5){
                break;
            }
        } 
for(int i = 127; i >0; i = (i-1)/2){
            LATD = i;
            __delay_ms(500);
            CheckButton();
            if(efectoActual !=5){
                break;
            }
        } 
}

void efecto6(){
for(int i = 0; i < 4; i = i++){
            LATD += pow(2,(7-i))+ pow(2,(i));
            __delay_ms(500);
            CheckButton();
            if(efectoActual !=6){
                break;
            }
        } 
for(int i = 3; i >= 0; i = i--){
            LATD -= pow(2,(7-i))+ pow(2,(i));
            __delay_ms(500);
            CheckButton();
            if(efectoActual !=6){
                break;
            }
        }
}

void efecto7(){
unsigned char snake[4] = {0x01, 0x03, 0x07, 0x0F}; // Etapas de entrada de la serpiente
    unsigned char mask = 0xFF; // Máscara para limitar a 8 bits
    
    while (1) {
        // Fase 1: Entrada de la serpiente
        for (int i = 0; i < 4; i++) {
            LATD = snake[i]; // Muestra la etapa actual de entrada
            __delay_ms(200);
        }
        
        // Fase 2: Desplazamiento por los LEDs
        for (int i = 0; i < 8 - 4 + 1; i++) {
            LATD = (0x0F << i) & mask; // Desplaza el cuerpo de la serpiente
            __delay_ms(200);
        }
        
        // Fase 3: Salida de la serpiente
        for (int i = 0; i < 4; i++) {
            LATD = (0x0F << (8 - 4 + i)) & mask; // Apaga progresivamente la cola
            __delay_ms(200);
        }

        // Fase 4: Reinicio (apaga todos los LEDs)
        LATD = 0x00;
        __delay_ms(500);
    }
}

void efecto7B(){
    for(int i = 128; i>= 16; i/=2){
        LATD+=i;
        __delay_ms(500);
        CheckButton();
        if(efectoActual !=7){
                break;
            }
    }
    for(int i = 0; i< 4;i++){
        LATD/=2;
        __delay_ms(500);
        CheckButton();
        if(efectoActual !=7){
                break;
            }
    }
    for(int i=8; i >=1; i/=2){
        LATD-= i;
        __delay_ms(500);
        CheckButton();
        if(efectoActual !=7){
                break;
            }
    }
}

void efecto8(){
    for (int i = 0; i < 255; i++) {
    LATD = i;
    __delay_ms(250);
    CheckButton();
    if(efectoActual !=8){
                break;
            }
    }
    
}

void CheckButton(){
    if(Boton == ON){
        efectoActual++;
    }
}
   
