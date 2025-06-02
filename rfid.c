/*
 * File: rfid_security_system.c
 * Author: Grok
 * Description: Système de sécurité RFID avec PIC16F877A
 * - Lecture de badge RFID via UART
 * - Commande d'un relais pour verrouillage de porte
 * - Affichage sur LCD 16x2
 * - Programmation en C pour MPLAB X avec XC8
 * Target: PIC16F877A
 * Compiler: XC8
 */

#include <xc.h>
#include <string.h>
#include <stdio.h>

// Configuration bits
#pragma config FOSC = HS        // Oscillateur HS
#pragma config WDTE = OFF       // Watchdog Timer désactivé
#pragma config PWRTE = ON       // Power-up Timer activé
#pragma config BOREN = ON       // Brown-out Reset activé
#pragma config LVP = OFF        // Low-Voltage Programming désactivé
#pragma config CPD = OFF        // Data EEPROM protection désactivée
#pragma config WRT = OFF        // Flash Program Memory Write désactivé
#pragma config CP = OFF         // Code Protection désactivée

// Définitions des fréquences et délais
#define _XTAL_FREQ 20000000 // Fréquence oscillateur 20MHz
#define BAUDRATE 9600       // Vitesse UART pour RFID

// Définitions des pins
#define RELAY_PIN RB0       // Pin pour le relais (RB0)
#define LCD_RS RB1          // Pin RS du LCD
#define LCD_EN RB2          // Pin Enable du LCD
#define LCD_D4 RB4          // Pin D4 du LCD
#define LCD_D5 RB5          // Pin D5 du LCD
#define LCD_D6 RB6          // Pin D6 du LCD
#define LCD_D7 RB7          // Pin D7 du LCD

// Prototypes des fonctions
void LCD_Init(void);
void LCD_Command(unsigned char);
void LCD_Char(unsigned char);
void LCD_String(const char*);
void LCD_Clear(void);
void UART_Init(void);
unsigned char UART_Read(void);
void UART_Write(unsigned char);
void UART_Write_Text(const char*);
void System_Init(void);
void Check_RFID(void);

// Base de données des tags RFID autorisés (exemple)
const char *authorized_tags[] = {
    "1A2B3C4D5E",
    "5F6G7H8I9J",
    "A1B2C3D4E5",
    NULL // Terminateur de la liste
};

// Buffer pour stocker le tag RFID lu
char rfid_tag[11];

// Fonction principale
void main(void) {
    System_Init();
    
    // Message d'accueil
    LCD_Clear();
    LCD_String("Systeme Securite");
    __delay_ms(2000);
    LCD_Clear();
    LCD_String("Scan Badge...");
    
    while(1) {
        Check_RFID();
    }
}

// Initialisation du système
void System_Init(void) {
    // Configuration des ports
    TRISB = 0x00;      // Port B en sortie (LCD + Relais)
    TRISC6 = 0;        // TX en sortie
    TRISC7 = 1;        // RX en entrée
    
    // Initialisation des périphériques
    LCD_Init();
    UART_Init();
    
    // Initialisation du relais (éteint)
    RELAY_PIN = 0;
}

// Initialisation du LCD en mode 4 bits
void LCD_Init(void) {
    __delay_ms(15);    // Attente pour stabilisation du LCD
    LCD_Command(0x02); // Mode 4 bits
    LCD_Command(0x28); // 2 lignes, 5x7 matrix
    LCD_Command(0x0C); // Affichage ON, curseur OFF
    LCD_Command(0x06); // Déplacement du curseur à droite
    LCD_Command(0x01); // Effacer l'écran
    __delay_ms(2);
}

// Envoi d'une commande au LCD
void LCD_Command(unsigned char cmd) {
    LCD_RS = 0; // Mode commande
    LCD_D4 = (cmd >> 4) & 0x01;
    LCD_D5 = (cmd >> 5) & 0x01;
    LCD_D6 = (cmd >> 6) & 0x01;
    LCD_D7 = (cmd >> 7) & 0x01;
    LCD_EN = 1;
    __delay_us(1);
    LCD_EN = 0;
    __delay_us(1);
    
    LCD_D4 = cmd & 0x01;
    LCD_D5 = (cmd >> 1) & 0x01;
    LCD_D6 = (cmd >> 2) & 0x01;
    LCD_D7 = (cmd >> 3) & 0x01;
    LCD_EN = 1;
    __delay_us(1);
    LCD_EN = 0;
    __delay_us(100);
}

// Envoi d'un caractère au LCD
void LCD_Char(unsigned char data) {
    LCD_RS = 1; // Mode données
    LCD_D4 = (data >> 4) & 0x01;
    LCD_D5 = (data >> 5) & 0x01;
    LCD_D6 = (data >> 6) & 0x01;
    LCD_D7 = (data >> 7) & 0x01;
    LCD_EN = 1;
    __delay_us(1);
    LCD_EN = 0;
    __delay_us(1);
    
    LCD_D4 = data & 0x01;
    LCD_D5 = (data >> 1) & 0x01;
    LCD_D6 = (data >> 2) & 0x01;
    LCD_D7 = (data >> 3) & 0x01;
    LCD_EN = 1;
    __delay_us(1);
    LCD_EN = 0;
    __delay_us(40);
}

// Affichage d'une chaîne sur le LCD
void LCD_String(const char *str) {
    while(*str) {
        LCD_Char(*str++);
    }
}

// Effacement du LCD
void LCD_Clear(void) {
    LCD_Command(0x01);
    __delay_ms(2);
}

// Initialisation de l'UART
void UART_Init(void) {
    // Configuration pour 9600 baud @ 20MHz
    SPBRG = (_XTAL_FREQ/(16UL * BAUDRATE)) - 1;
    TXSTA = 0x24; // TX enable, BRGH=1
    RCSTA = 0x90; // Serial port enable, RX enable
}

// Lecture d'un octet depuis l'UART
unsigned char UART_Read(void) {
    while(!PIR1bits.RCIF); // Attendre réception
    if(RCSTA & 0x06) {     // Vérifier erreurs
        RCSTA = 0;         // Reset UART en cas d'erreur
        RCSTA = 0x90;
    }
    return RCREG;
}

// Écriture d'un octet sur l'UART
void UART_Write(unsigned char data) {
    while(!TXSTAbits.TRMT);
    TXREG = data;
}

// Écriture d'une chaîne sur l'UART
void UART_Write_Text(const char *text) {
    while(*text) {
        UART_Write(*text++);
    }
}

// Vérification du tag RFID
void Check_RFID(void) {
    unsigned char i = 0;
    unsigned char received_char;
    
    // Lecture du tag (10 caractères + terminateur)
    while(i < 10) {
        received_char = UART_Read();
        if((received_char >= '0' && received_char <= '9') || 
           (received_char >= 'A' && received_char <= 'F')) {
            rfid_tag[i] = received_char;
            i++;
        }
    }
    rfid_tag[i] = '\0'; // Terminateur
    
    // Vérification si le tag est autorisé
    i = 0;
    while(authorized_tags[i] != NULL) {
        if(strcmp(rfid_tag, authorized_tags[i]) == 0) {
            // Tag autorisé
            LCD_Clear();
            LCD_String("Acces Autorise");
            RELAY_PIN = 1; // Activer relais
            __delay_ms(3000); // Garder porte ouverte 3s
            RELAY_PIN = 0; // Désactiver relais
            LCD_Clear();
            LCD_String("Scan Badge...");
            return;
        }
        i++;
    }
    
    // Tag non autorisé
    LCD_Clear();
    LCD_String("Acces Refuse");
    __delay_ms(2000);
    LCD_Clear();
    LCD_String("Scan Badge...");
}

// Gestion des interruptions (non utilisée ici mais incluse pour complétude)
void __interrupt() ISR(void) {
    // À implémenter si besoin d'interruptions
}