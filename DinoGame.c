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
#define BotonL PORTBbits.RB4 // Mueve left al dino
#define BotonR PORTBbits.RB5 // Mueve right al dino
#define MAX_X 11
#define MIN_X 0

// Variables globales
int score = 0;
int lives = 3;
int gameActive = 0;
int dinosaurioPos = 1;  // 1 = Suelo, 0 = Arriba
int baseDelay = 200;    // Velocidad base (ms)
int cactusPos = -1;     // Posición del cactus (-1 = fuera de pantalla)
int pajaroPos = -1;     // Posición del pájaro (-1 = fuera de pantalla)
int dinoX = 0;
int dinoY = 1;
int prevDinoX = 0;
int prevDinoY = 1;

// Prototipos de funciones
void initializeSystem(void);
void initializeLCD(void);
void createCustomChars(void);
void displayStartScreen(void);
void displayGameScreen(void);
void moveDinosaurio(void);
void moveCactus(void);
void movePajaro(void);
void handleCollision(void);
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
            moveDinosaurio(); // Actualiza la posición del dinosaurio según botones
            moveCactus();     // Mueve el cactus incrementalmente
            movePajaro();     // Mueve el pájaro incrementalmente
            checkCollision(); // Verifica colisiones en cada paso
            updateUI();       // Actualiza la interfaz

            // Aumentar velocidad cada 2 puntos en un 20%
            static int lastSpeedIncreaseScore = -1;
            if (score % 2 == 0 && score > 0 && lastSpeedIncreaseScore != score) {
                baseDelay = (baseDelay * 4) / 5; // Reducir el retardo en 20%
                if (baseDelay < 50) baseDelay = 50; // Límite mínimo de velocidad
                lastSpeedIncreaseScore = score;
            }

            // Mostrar créditos al alcanzar 16 puntos
            if (score > 16) {
                displayCredits();
                gameActive = 2; // Detener el juego
            }

            // Retardo basado en baseDelay
            for (int i = 0; i < baseDelay / 10; i++) {
                __delay_ms(10);
            }
        } else if (gameActive == 2) {
            // Mantener el estado de Game Over o Créditos y permitir reinicio
            check_buttons(); // Solo verifica botones para reiniciar
            __delay_ms(10);  // Pequeño retardo para evitar consumo excesivo
        }
    }
}

// Inicialización del sistema
void initializeSystem() {
    OSCCONbits.IRCF = 0b111; // 8 MHz internal oscillator
    TRISD = OUT;
    TRISBbits.RB0 = IN;
    TRISBbits.RB1 = IN;
    TRISBbits.RB3 = IN; // Nuevo botón en RB3 para START/RESET7
    TRISBbits.RB4 = IN;
    TRISBbits.RB5 = IN;
    TRISCbits.RC0 = OUT;
    INTCON2bits.RBPU = 1; // Deshabilitar pull-up interno
}

// Inicialización del LCD
void initializeLCD() {
    iniLCD(16, 2);  // LCD de 16x2
    LCDnoBlink();
    LCDnoCursor();
}

// Creación de caracteres personalizados
void createCustomChars() {
    unsigned char dinosaurio[8] = {0b00000, 0b00110, 0b00110, 0b00110, 0b01110, 0b01110, 0b11111, 0b01110};
    unsigned char dinosaurio2[8] = {0b00000,0b00110,0b00110,0b00110,0b01110,0b01110,0b11111,0b10001}; //El sprite 2 
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
    LCDcommand(SetCGRAM | 0x20);
    GenChar((char *)dinosaurio2);

}

// Mostrar pantalla de inicio
void displayStartScreen() {
    LCDclear();
    LCDMoveCursor(3, 0);
    LCDprint(8, "Presiona", 100);
    LCDMoveCursor(3, 1);
    LCDprint(7, "  START  ", 100);
}

// Mostrar pantalla del juego con línea vertical
void displayGameScreen() {
    LCDclear();

    // Dibujar línea vertical en la columna 12
    LCDMoveCursor(12, 0);
    LCDchar('|'); // Línea vertical en la primera fila
    LCDMoveCursor(12, 1);
    LCDchar('|'); // Línea vertical en la segunda fila

    // Mostrar el dinosaurio en la posición inicial (suelo)
    moveDinosaurio();

    // Mostrar la UI (vidas y puntaje)
    updateUI();
}

// Movimiento del dinosaurio controlado por botones
uint8_t dinoFrame = 0; // Track del dino 1 o 2

void moveDinosaurio() {
    // Clear only the previous position
    LCDMoveCursor(prevDinoX, prevDinoY);
    LCDchar(' ');

    // Draw dino at new position
    LCDMoveCursor(dinoX, dinoY);
    LCDchar(dinoFrame == 0 ? 0x00 : (uint8_t)4); // Animar

    // Alternate animation frame
    dinoFrame = !dinoFrame;

    // Save current position as previous for next move
    prevDinoX = dinoX;
    prevDinoY = dinoY;
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
            LCDMoveCursor(0, 1);
            LCDchar(' ');
            score++;
            cactusPos = -1;
        }
    }
    delayCounter++;
}

// Movimiento del pájaro
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
            LCDMoveCursor(0, 0);
            LCDchar(' ');
            score++;
            pajaroPos = -1;
        }
    }
    delayCounter++;
}

void handleCollision() {
    // Resetear obstaculos
    for (int i = 0; i < 12; i++) {
        LCDMoveCursor(i, 0);
        LCDchar(' ');
        LCDMoveCursor(i, 1);
        LCDchar(' ');
    }
    // Redraw vertical divider
    LCDMoveCursor(12, 0); LCDchar('|');
    LCDMoveCursor(12, 1); LCDchar('|');

    // -vida y reset posicional
    lives--;
    cactusPos = -1;
    pajaroPos = -1;
    dinoX = 0;
    dinoY = 1;
    moveDinosaurio();
    updateUI();

    if (lives <= 0) {
        gameOver();
    }
}

// Verificación de colisiones
void checkCollision() {
    // Cactus collision (bottom row)
    if (dinoY == 1 && cactusPos == dinoX) {
        handleCollision();
    }

    // Bird collision (top row)
    if (dinoY == 0 && pajaroPos == dinoX) {
        handleCollision();
    }
}

// Pantalla de "Game Over"
void gameOver() {
    LCDclear();
    LCDMoveCursor(3, 0);
    LCDprint(8, "GAMEOVER", 50); // Ajustado para que el mensaje sea más corto y centrado
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
                LCDchar(0x03); // Carácter de corazón
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

        // Mostrar el puntaje con dos dígitos
        LCDMoveCursor(14, 1);
        if (score < 10) {
            LCDchar('0'); // Agregar cero a la izquierda
            LCDMoveCursor(15, 1);
            LCDint(score); // Escribir el dígito
        } else {
            LCDint(score); // Escribir el número completo (ocupará 14 y 15)
        }

        lastScore = score;
    }
}

// Detección de los botones en tiempo real
void check_buttons() {
    if (Boton == ON && gameActive == ON) {
        if (dinoY > 0) {
            dinoY--;
            moveDinosaurio();
        }
    } else if (Boton2 == ON && gameActive == ON) {
        if (dinoY < 1) {
            dinoY++;
            moveDinosaurio();
        }
    } else if (BotonL == ON && gameActive == ON) {
        dinoX--;
        moveDinosaurio();
    } else if (BotonR == ON && gameActive == ON) {
        dinoX++;
        moveDinosaurio();
    }

    // Implementamos el cheque de los limites
    if (gameActive == ON && (dinoX < MIN_X || dinoX > MAX_X)) {
        handleCollision(); // Pierde vida el dino
    }

    if (Boton3 == ON) {
        __delay_ms(50);
        if (Boton3 == ON) {
            while (Boton3 == ON);
            resetGame();
            gameActive = 1;
        }
    }
}




// Función para reiniciar el juego
void resetGame() {
    gameActive = 1;
    score = 0;
    lives = 3;
    dinosaurioPos = 1;
    baseDelay = 200; // Restablecer velocidad base
    cactusPos = -1;  // Resetear posiciones de obstáculos
    pajaroPos = -1;

    // Limpiar la pantalla y redibujar desde cero
    LCDclear();
    // Dibujar línea vertical en la columna 12
    LCDMoveCursor(12, 0);
    LCDchar('|');
    LCDMoveCursor(12, 1);
    LCDchar('|');

    // Mostrar el dinosaurio en la posición inicial
    moveDinosaurio();

    // Forzar actualización de la UI con los nuevos valores
    updateUI();
}

// Pantalla de Créditos
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
    __delay_ms(5000); // Mostrar créditos durante 3 segundos
    LCDclear();
    LCDprint(10, "Play Again    ", 100);
    __delay_ms(5000); // Mostrar créditos durante 3 segundos

    resetGame(); // Reiniciar el juego automáticamente después de los créditos
}