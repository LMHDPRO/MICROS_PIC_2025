#include "LCD.h"

//Funciones LCD
void iniLCD(unsigned char cols, unsigned char lines){
    TRISD = 0;    // PortD as output (for LCD)
    TRISEbits.RE0 = 0; // RE0 output for RS
    TRISEbits.RE1 = 0; // RE1 output for EN
    TRISEbits.RE2 = 0; // RE2 output (alternate for RS)
    
    _displaycontrol = DispOnCB;
    _cols = cols;
    _numlines = lines;
      
    __delay_ms(20); //1. Esperar mas de 15ms
    LCDcommand(Set8); //2. Comando set 8 bits
    __delay_ms(5); //3. Esperar mas de 4.1ms
    LCDcommand(Set8); //4. Comando set 8 bits
    __delay_ms(1); //5. Esperar mas de 100us
    LCDcommand(Set8); //6. Comando set 8 bits
    LCDcommand(Set8_2lin); //N = 1 (2 lines), F=0 (5x8)    
    LCDcommand(_displaycontrol); //D<2> ON; C<1> ON; B<0> ON;
    LCDclear();
    LCDcommand(IncNoShif); //I/D = 1, S=0
    LCDhome();
    return;
}

void LCDcommand(char a){
    RS = 0; //RE0 --> RS=0
    LATD = a;
    EN = 1; //RE1 --> E=1
    __delay_ms(1);
    EN = 0; //RE1 --> E=0
    __delay_ms(1);
    return;
}

void LCDchar(char a){
    RS = 1;
    LATD = a; //Valor ASCII
    EN = 1; //RE1 --> E=1
    __delay_us(40);
    EN = 0; //RE1 --> E=0
    return;
}

void LCDprint(int size, char a[], int wait_ms){
    for(int i = 0; i<size; i++){
        LCDchar(a[i]);
        for(int j = 0; j<wait_ms; j++){
            __delay_ms(1);
        }
    }
    return;
}

void LCDMoveCursor(char x, char y){
    char a;
    if(y <= 1 && x <= 39){
        if(y ==0){
            a = x;
        }else{
            a = x + 64;
        }
        a = a | SetDDRAM;
    }
    LCDcommand(a);
    return;
}

void LCDScroll(char dir, char inc, char del){
    char command;
    if(dir==0){
        command = DispSfL;
    }else{
        command = DispSfR;
    }
    for(char i = 0; i < inc; i++){
        LCDcommand(command);
        for(char j = 0; j < del; j++)
            __delay_ms(1);
    }
    return;
}

void GenChar(char a[8]){
    for(int i = 0; i < 8; i++){
        LCDchar(a[i]);
    }
    return;
}

void LCDint(long int a){
    char U=0;
    char D=0;
    char C=0;
    char UM=0;
    char DM=0;
    char CM=0;
    if(a < 0){
        LCDchar('-');
        a *= -1;
    }
    if(a < 10){   
        U = (char) a;
        U = U + '0';
        LCDchar(U);
    }else if(a < 100){
        U = (char) a%10;      
        D = (char) (a - U)/10;
        U = U + '0';
        D = D + '0';
        LCDchar(D);
        LCDchar(U);
    }else if(a < 1000){
        U = (char) a%10;
        D = (char) (a%100 - U)/10;
        C = (char) (a - D*10 - U)/100;
        U = U + '0';
        D = D + '0';
        C = C + '0';
        LCDchar(C);
        LCDchar(D);
        LCDchar(U);
    }else if(a < 10000){  
        U = (char) a%10;  
        D = (char) (a%100 - U)/10;  
        C = (char) (a%1000 - D*10 - U)/100; 
        UM = (char) (a - C*100 - D*10 - U)/1000;
        U = U + '0';
        D = D + '0';
        C = C + '0';
        UM = UM + '0';
        LCDchar(UM);
        LCDchar(C);
        LCDchar(D);
        LCDchar(U);
    }else if(a < 100000){
        char U = (char) a%10;
        char D = (char) (a%100 - U)/10;
        char C = (char) (a%1000 - D*10 - U)/100;
        char UM = (char) (a%10000 - C*100 - D*10 - U)/1000;
        char DM = (char) (a - UM*1000 - C*100 - D*10 - U)/10000;
        U = U + '0';
        D = D + '0';
        C = C + '0';
        UM = UM + '0';
        DM = DM + '0';
        LCDchar(DM);
        LCDchar(UM);
        LCDchar(C);
        LCDchar(D);
        LCDchar(U);
    }else{
        char U = (char) a%10;
        char D = (char) (a%100 - U)/10;
        char C = (char) (a%1000 - D*10 - U)/100;
        char UM = (char) (a%10000 - C*100 - D*10 - U)/1000;
        char DM = (char) (a%100000 - UM*1000 - C*100 - D*10 - U)/10000;
        char CM = (char) (a - DM*10000 + UM*1000 - C*100 - D*10 - U)/100000;
        U = U + '0';
        D = D + '0';
        C = C + '0';
        UM = UM + '0';
        DM = DM + '0';
        CM = CM + '0';
        LCDchar(CM);
        LCDchar(DM);
        LCDchar(UM);
        LCDchar(C);
        LCDchar(D);
        LCDchar(U);
    }
    return;
}

void LCDclear(){
    LCDcommand(ClearDisp);// clear display, set cursor position to zero
    __delay_us(2000);  // this command takes a long time!
    return;
}

void LCDhome(){
    LCDcommand(ReHome);  // set cursor position to zero
    __delay_us(2000);  // this command takes a long time!
    return;
}

// Turn the display on/off (quickly)
void LCDnoDisplay() {
    _displaycontrol &= ~DispOn;
    LCDcommand(DispOff | _displaycontrol);
    return;
}

void LCDdisplay() {
    _displaycontrol |= DispOn;
    LCDcommand(DispOff | _displaycontrol);
    return;
}

// Turns the underline cursor on/off
void LCDnoCursor() {
    _displaycontrol &= ~LCD_CURSORON;
    LCDcommand(DispOff | _displaycontrol);
    return;
}
void LCDcursor() {
    _displaycontrol |= LCD_CURSORON;
    LCDcommand(DispOff | _displaycontrol);
    return;
}

// Turn on and off the blinking cursor
void LCDnoBlink() {
    _displaycontrol &= ~LCD_BLINKON;
    LCDcommand(DispOff | _displaycontrol);
    return;
}
void LCDblink() {
    _displaycontrol |= LCD_BLINKON;
    LCDcommand(DispOff | _displaycontrol);
    return;
}