#ifndef LCD_H
#define    LCD_H

#include <xc.h> 

#define ClearDisp 0b00000001
#define ReHome    0b00000010
#define IncShif   0b00000111
#define IncNoShif 0b00000110
#define DecShif   0b00000101
#define DecNoShif 0b00000100
#define DispOff   0b00001000
#define DispOn    0b00001100
#define DispOnC   0b00001110
#define DispOnB   0b00001101
#define DispOnCB  0b00001111
#define DispSfL   0b00011000
#define DispSfR   0b00011100
#define CursSfL   0b00010000
#define CursSfR   0b00010100
#define Set8      0b00110000
#define Set4      0b00100000
#define Set8_2lin 0b00111000
#define Set4_2lin 0b00101000
#define SetCGRAM  0b01000000
#define SetDDRAM  0b10000000

#ifndef _XTAL_FREQ
#define _XTAL_FREQ 8000000
#endif    

#define RS LATEbits.LE0 //LE2 //(Alternative pin)
#define EN LATEbits.LE1

#define LCD_CURSORON 0x02
#define LCD_BLINKON 0x01

#define LINE_UP 0
#define LINE_DOWN 1
#define LEFT 0
#define RIGHT 1

unsigned char _cols;
unsigned char _numlines;
unsigned char _displaycontrol;

void iniLCD(unsigned char cols, unsigned char lines);
void LCDcommand(char a);
void LCDchar(char a);
void LCDprint(int size, char a[], int wait_ms);
void LCDMoveCursor(char x, char y);
void LCDScroll(char dir, char inc, char del);
void GenChar(char a[8]);
void LCDint(long int a);
void LCDclear(void);
void LCDhome(void);
void LCDnoDisplay(void);
void LCDdisplay(void);
void LCDnoCursor(void); 
void LCDcursor(void);
void LCDnoBlink(void);
void LCDblink(void);

#endif    /* LCD_H */