#include <xc.h>
#include "LCD.h"

#pragma config PLLDIV = 1       // PLL Prescaler (for 8MHz internal oscillator)
#pragma config CPUDIV = OSC1_PLL2 // System Clock Postscaler
#pragma config USBDIV = 2       // USB clock source (if using USB, adjust if not)
#pragma config FOSC = INTOSCIO_EC // Internal oscillator, EC mode
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor disabled
#pragma config IESO = OFF       // Internal/External Oscillator Switchover disabled
#pragma config PWRT = OFF       // Power-up Timer disabled
#pragma config BOR = OFF        // Brown-out Reset disabled
#pragma config WDT = OFF        // Watchdog Timer disabled
#pragma config PBADEN = OFF     // PORTB A/D disabled
#pragma config LVP = OFF        // Low-Voltage Programming disabled
#pragma config MCLRE = ON       // MCLR pin enabled

#define _XTAL_FREQ 8000000
#define IN 1
#define OUT 0
#define ON 1
#define OFF 0
#define Boton PORTBbits.RB0  // Sube el dinosaurio
#define Boton2 PORTBbits.RB1 // Baja el dinosaurio
#define Boton3 PORTBbits.RB3 // Inicia y reinicia el juego

// Variables globales
int score = 0;
int lives = 3;
int gameActive = 0;
int dinosaurioPos = 1;  // 1 = Suelo, 0 = Arriba
int baseDelay = 200;    // Velocidad base (ms)
int cactusPos = -1;     // Posici�n del cactus (-1 = fuera de pantalla)
int pajaroPos = -1;     // Posici�n del p�jaro (-1 = fuera de pantalla)

// Prototipos de funciones
void initializeSystem(void);
void initializeLCD(void);
void createCustomChars(void);
void displayStartScreen(void);
void displayGameScreen(void);
void moveDinosaurio(void);
void moveCactus(void);
void movePajaro(void);
void checkCollision(void);
void gameOver(void);
void updateUI(void);
void check_buttons(void);
void resetGame(void);
void displayCredits(void);

void main(void) {
    initializeSystem();
    initializeLCD();
    LCDnoCursor();  // Desactivar el cursor
    LCDnoBlink();   // Desactivar el parpadeo
    createCustomChars();
    displayStartScreen();

    while (1) {
        check_buttons();  // Verifica botones en tiempo real

        if (gameActive == 1) {
            moveDinosaurio(); // Actualiza la posici�n del dinosaurio seg�n botones
            moveCactus();     // Mueve el cactus incrementalmente
            movePajaro();     // Mueve el p�jaro incrementalmente
            checkCollision(); // Verifica colisiones en cada paso
            updateUI();       // Actualiza la interfaz

            // Aumentar velocidad cada 2 puntos en un 20%
            static int lastSpeedIncreaseScore = -1;
            if (score % 2 == 0 && score > 0 && lastSpeedIncreaseScore != score) {
                baseDelay = (baseDelay * 4) / 5; // Reducir el retardo en 20%
                if (baseDelay < 50) baseDelay = 50; // L�mite m�nimo de velocidad
                lastSpeedIncreaseScore = score;
            }

            // Mostrar cr�ditos al alcanzar 16 puntos
            if (score > 16) {
                displayCredits();
                gameActive = 2; // Detener el juego
            }

            // Retardo basado en baseDelay
            for (int i = 0; i < baseDelay / 10; i++) {
                __delay_ms(10);
            }
        } else if (gameActive == 2) {
            // Mantener el estado de Game Over o Cr�ditos y permitir reinicio
            check_buttons(); // Solo verifica botones para reiniciar
            __delay_ms(10);  // Peque�o retardo para evitar consumo excesivo
        }
    }
}

// Inicializaci�n del sistema
void initializeSystem() {
    OSCCONbits.IRCF = 0b111; // 8 MHz internal oscillator
    TRISD = OUT;
    TRISBbits.RB0 = IN;
    TRISBbits.RB1 = IN;
    TRISBbits.RB3 = IN; // Nuevo bot�n en RB3 para START/RESET
    TRISCbits.RC0 = OUT;
    INTCON2bits.RBPU = 1; // Deshabilitar pull-up interno
}

// Inicializaci�n del LCD
void initializeLCD() {
    iniLCD(16, 2);  // LCD de 16x2
    LCDnoBlink();
    LCDnoCursor();
}

// Creaci�n de caracteres personalizados
void createCustomChars() {
    unsigned char dinosaurio[8] = {0b00000, 0b00110, 0b00110, 0b00110, 0b01110, 0b01110, 0b11111, 0b01110};
    unsigned char cactus[8] = {0b00100, 0b00101, 0b10101, 0b10101, 0b10111, 0b11100, 0b00100, 0b00100};
    unsigned char pajaro[8] = {0b00000, 0b00000, 0b00110, 0b10101, 0b11111, 0b01110, 0b00100, 0b00000};
    unsigned char corazon[8] = {0b00000, 0b01010, 0b11111, 0b11111, 0b11111, 0b01110, 0b00100, 0b00000};

    LCDcommand(SetCGRAM | 0x00);
    GenChar((char *)dinosaurio);
    LCDcommand(SetCGRAM | 0x08);
    GenChar((char *)cactus);
    LCDcommand(SetCGRAM | 0x10);
    GenChar((char *)pajaro);
    LCDcommand(SetCGRAM | 0x18);
    GenChar((char *)corazon);
}

// Mostrar pantalla de inicio
void displayStartScreen() {
    LCDclear();
    LCDMoveCursor(3, 0);
    LCDprint(8, "Presiona", 100);
    LCDMoveCursor(3, 1);
    LCDprint(7, "START", 100);
}

// Mostrar pantalla del juego con l�nea vertical
void displayGameScreen() {
    LCDclear();

    // Dibujar l�nea vertical en la columna 12
    LCDMoveCursor(12, 0);
    LCDchar('|'); // L�nea vertical en la primera fila
    LCDMoveCursor(12, 1);
    LCDchar('|'); // L�nea vertical en la segunda fila

    // Mostrar el dinosaurio en la posici�n inicial (suelo)
    moveDinosaurio();

    // Mostrar la UI (vidas y puntaje)
    updateUI();
}

// Movimiento del dinosaurio controlado por botones
void moveDinosaurio() {
    if (dinosaurioPos == 0) {
        LCDMoveCursor(0, 1);
        LCDchar(' '); // Borra en suelo
        LCDMoveCursor(0, 0);
        LCDchar(0x00); // Muestra arriba
    } else {
        LCDMoveCursor(0, 0);
        LCDchar(' '); // Borra arriba
        LCDMoveCursor(0, 1);
        LCDchar(0x00); // Muestra en suelo
    }
}

// Movimiento del cactus
void moveCactus() {
    static int delayCounter = 0;

    if (cactusPos == -1 && pajaroPos == -1 && delayCounter >= (baseDelay / 10)) {
        cactusPos = 11;
        delayCounter = 0;
    }

    if (cactusPos >= 0) {
        if (cactusPos < 11) {
            LCDMoveCursor(cactusPos + 1, 1);
            LCDchar(' ');
        }
        LCDMoveCursor(cactusPos, 1);
        LCDchar(0x01);
        cactusPos--;

        if (cactusPos < 0) {
            score++;
            cactusPos = -1;
        }
    }
    delayCounter++;
}

// Movimiento del p�jaro
void movePajaro() {
    static int delayCounter = 0;

    if (pajaroPos == -1 && cactusPos == -1 && delayCounter >= (baseDelay / 10)) {
        pajaroPos = 11;
        delayCounter = 0;
    }

    if (pajaroPos >= 0) {
        if (pajaroPos < 11) {
            LCDMoveCursor(pajaroPos + 1, 0);
            LCDchar(' ');
        }
        LCDMoveCursor(pajaroPos, 0);
        LCDchar(0x02);
        pajaroPos--;

        if (pajaroPos < 0) {
            score++;
            pajaroPos = -1;
        }
    }
    delayCounter++;
}

// Verificaci�n de colisiones
void checkCollision() {
    // Colisi�n con cactus (dinosaurio en el suelo, fila 1)
    if (dinosaurioPos == 1 && cactusPos == 0) {
        // Limpiar toda la pantalla de juego (columnas 0 a 11)
        for (int i = 0; i < 12; i++) {
            LCDMoveCursor(i, 0);
            LCDchar(' ');
            LCDMoveCursor(i, 1);
            LCDchar(' ');
        }
        // Redibujar la l�nea vertical en la columna 12
        LCDMoveCursor(12, 0);
        LCDchar('|');
        LCDMoveCursor(12, 1);
        LCDchar('|');

        lives--;
        cactusPos = -1;
        pajaroPos = -1;
        dinosaurioPos = 1;
        moveDinosaurio();
        updateUI();

        if (lives <= 0) {
            gameOver();
        }
    }
    // Colisi�n con p�jaro (dinosaurio en el aire, fila 0)
    if (dinosaurioPos == 0 && pajaroPos == 0) {
        // Limpiar toda la pantalla de juego (columnas 0 a 11)
        for (int i = 0; i < 12; i++) {
            LCDMoveCursor(i, 0);
            LCDchar(' ');
            LCDMoveCursor(i, 1);
            LCDchar(' ');
        }
        // Redibujar la l�nea vertical en la columna 12
        LCDMoveCursor(12, 0);
        LCDchar('|');
        LCDMoveCursor(12, 1);
        LCDchar('|');

        lives--;
        pajaroPos = -1;
        cactusPos = -1;
        dinosaurioPos = 1;
        moveDinosaurio();
        updateUI();

        if (lives <= 0) {
            gameOver();
        }
    }
}

// Pantalla de "Game Over"
void gameOver() {
    LCDclear();
    LCDMoveCursor(3, 0);
    LCDprint(8, "GAMEOVER", 50); // Ajustado para que el mensaje sea m�s corto y centrado
    gameActive = 2;
}

// UI Mejorado con corazones y puntaje
void updateUI() {
    static int lastLives = -1;
    static int lastScore = -1;

    if (lastLives != lives) {
        LCDMoveCursor(13, 0);
        for (int i = 0; i < 3; i++) {
            if (i < lives) {
                LCDchar(0x03); // Car�cter de coraz�n
            } else {
                LCDchar(' ');
            }
        }
        lastLives = lives;
    }

    if (lastScore != score) {
        LCDMoveCursor(13, 1);
        LCDprint(3, "P:", 0); // Mostrar "P:" correctamente

        // Limpiar las posiciones del puntaje (columnas 14 y 15)
        LCDMoveCursor(14, 1);
        LCDchar(' ');
        LCDMoveCursor(15, 1);
        LCDchar(' ');

        // Mostrar el puntaje con dos d�gitos
        LCDMoveCursor(14, 1);
        if (score < 10) {
            LCDchar('0'); // Agregar cero a la izquierda
            LCDMoveCursor(15, 1);
            LCDint(score); // Escribir el d�gito
        } else {
            LCDint(score); // Escribir el n�mero completo (ocupar� 14 y 15)
        }

        lastScore = score;
    }
}

// Detecci�n de los botones en tiempo real
void check_buttons() {
    if (Boton == ON) {  
        dinosaurioPos = 0;
        moveDinosaurio();
    } 
    if (Boton2 == ON) {  
        dinosaurioPos = 1;
        moveDinosaurio();
    }
    if (Boton3 == ON) {  
        __delay_ms(50);
        if (Boton3 == ON) {
            while (Boton3 == ON); // Esperar a que se suelte el bot�n
            resetGame();
        }
    }
}

// Funci�n para reiniciar el juego
void resetGame() {
    gameActive = 1;
    score = 0;
    lives = 3;
    dinosaurioPos = 1;
    baseDelay = 200; // Restablecer velocidad base
    cactusPos = -1;  // Resetear posiciones de obst�culos
    pajaroPos = -1;

    // Limpiar la pantalla y redibujar desde cero
    LCDclear();
    // Dibujar l�nea vertical en la columna 12
    LCDMoveCursor(12, 0);
    LCDchar('|');
    LCDMoveCursor(12, 1);
    LCDchar('|');

    // Mostrar el dinosaurio en la posici�n inicial
    moveDinosaurio();

    // Forzar actualizaci�n de la UI con los nuevos valores
    updateUI();
}

// Pantalla de Cr�ditos
void displayCredits() {
    LCDclear();
    LCDMoveCursor(4, 0);
    LCDprint(8, "You WON!", 100);
    __delay_ms(2000); // Mostrar "You WON!" durante 2 segundos

    LCDclear();
    LCDMoveCursor(2, 0);
    LCDprint(12, "Created by      ", 100);
    LCDMoveCursor(4, 1);
    LCDprint(8, "Comfy&LM   ", 100);
    __delay_ms(5000); // Mostrar cr�ditos durante 3 segundos
    LCDclear();
    LCDprint(8, "Play Again    ", 100);
    __delay_ms(5000); // Mostrar cr�ditos durante 3 segundos

    resetGame(); // Reiniciar el juego autom�ticamente despu�s de los cr�ditos
}