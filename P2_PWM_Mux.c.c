#include <xc.h>

#pragma config FOSC = INTOSCIO_EC
#pragma config WDT = OFF
#pragma config PBADEN = OFF 
#pragma config LVP = OFF

#define _XTAL_FREQ 8000000
#define IN 1
#define OUT 0
#define ON 1
#define OFF 0
#define Boton PORTBbits.RB0
#define Boton2 PORTBbits.RB1
#define D4 PORTCbits.RC6
#define D3 PORTCbits.RC7


#include <xc.h>

void check_button(void);
void configPWM(void);
void setDC1(int x);
void ej1();
void ej2();
void ej3();
void mux(void);

unsigned char val7[]={0x3F, 0x0A, 0x5D, 0x4F, 0x62, 0x6D, 0x7D, 0x0B, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x80};
int cuenta = 0;
char dig1 = 0;
char dig2 = 0;

void main(void) {
    OSCCONbits.IRCF = 7; // Osc = 8MHz
    TRISD = OUT;
    TRISBbits.RB0 = IN;
    TRISBbits.RB1 = IN;
    configPWM();
    TRISC = 0x00;
    LATD = val7[0];
    while(1){
        ej3();
    }
    return;
}  

void ej1() {
      //EJERCICIO 1 
    for(int i = 0; i <= 100; i+=0) {
            setDC1(i);
            __delay_ms(200);
        }
         for(int i = 100; i > 0; i-=10) {
            setDC1(i);
            __delay_ms(200);
        }
    return;
}

void ej2() {
    //EJERCICIO 2
    check_button();
    setDC1(cuenta*10);
    __delay_ms(200);
}

void ej3() {
    //EJERCICIO 3
        mux();
        mux();
        mux();
        mux();
        check_button();
        //LOGICA PARA CONTAR
        dig1 = cuenta;
        if(cuenta == 10) {
            cuenta = 0;
            dig1 = 0;
            dig2++;
            if(dig2 == 10) {
                dig1 = 9;
                dig2 = 9;
                cuenta = 9;
            }
        }
        if(cuenta < 0) {
            dig1 = 9;
            cuenta = 9;
            if(dig2 == 0) {
                dig1 = 0;
                dig2 = 0;
                cuenta = 0;
            } else {
                dig2--;
            }
        }
        return;
}

void mux() {
    //Poner el valor del digito 1 en PORTD
    LATD = val7[dig1]; 
    //Encender transistor 1
    D4 = ON;        
    //Esperar
    __delay_ms(45);
    //Apagar transistor 1
    D4 = OFF;
    //Poner el valor del digito 2 en PORTD
    LATD = val7[dig2];
    //Encender transistor 2
    D3 = ON;
    //Esperar
    __delay_ms(45);
    //Apagar transistor 2
    D3 = OFF;
}

void check_button() {
    if(Boton == ON) {
        cuenta++;
        if(cuenta > 10) {
            cuenta = 10;
        }
    }
     if(Boton2 == ON) {
        cuenta--;
    }
    
    return;
} 
void configPWM(){
    PR2 = 249;  //Valor para generar 500Hz --> Period=2ms
    //--Valores de Duty Cycle --
    setDC1(0);      //Configura DC del PWM1 a 0%
    TRISC = 0x00; //PortC salida
    T2CON = 0b00000111; //T2 ON, <1:0>preescaler = 16
    CCP1CON = 0b00001100; //<5:4>=<00> Configura el PWM
    CCP2CON = 0b00001100; //<5:4>=<00> Configura el PWM
    //--Fin de las configuraciones del PWM
    return;
}
void setDC1(int x){
    if(x < 0){   //limita el valor de 0 a 100
        x = 0;
    }
    if(x > 100){
        x = 100;
    }
    //calcula el valor a cargar despejando la fórmula
    //los valores de prescalador a 16 y Osc =8MHz son fijos
    int val = x*10;
    CCPR1L = (val - val%4)/4;  //8 bits altos
    CCP1CONbits.DC1B = val%4;  //2 bits bajos
}