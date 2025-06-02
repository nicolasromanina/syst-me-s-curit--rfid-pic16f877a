# Système de Sécurité RFID avec PIC16F877A

Ce projet implémente un système de sécurité basé sur la lecture de badges RFID utilisant un microcontrôleur PIC16F877A. Le système lit un badge RFID via UART, commande un relais pour le verrouillage/déverrouillage d'une porte, et affiche les informations sur un écran LCD 16x2. Le code est écrit en C avec le compilateur XC8 dans l'environnement MPLAB X.

## Fonctionnalités
- Lecture de tags RFID via UART (format : 10 caractères hexadécimaux)
- Vérification des tags RFID par rapport à une liste prédéfinie
- Commande d'un relais pour ouvrir/fermer une porte
- Affichage des messages d'état sur un écran LCD 16x2 (ex. "Accès Autorisé", "Accès Refusé")
- Messages d'accueil et d'invite pour scanner un badge
- Gestion robuste des erreurs UART

## Matériel requis
- **Microcontrôleur** : PIC16F877A
- **Module RFID** : Compatible UART (ex. MFRC522)
- **Écran LCD** : 16x2 (mode 4 bits)
- **Relais** : Pour le contrôle de la serrure de porte
- **Oscillateur externe** : 20 MHz (cristal)
- **Alimentation** : 5V pour le PIC et les périphériques
- **Câblage** : Fils de connexion, breadboard ou PCB
- **Programmateur** : PICkit ou équivalent pour programmer le PIC16F877A

## Connexions matérielles
| Périphérique | Pin PIC16F877A | Description |
|--------------|----------------|-------------|
| Relais       | RB0            | Commande du relais |
| LCD RS       | RB1            | Registre Select |
| LCD EN       | RB2            | Enable |
| LCD D4       | RB4            | Données bit 4 |
| LCD D5       | RB5            | Données bit 5 |
| LCD D6       | RB6            | Données bit 6 |
| LCD D7       | RB7            | Données bit 7 |
| UART TX      | RC6            | Transmission UART (vers RFID) |
| UART RX      | RC7            | Réception UART (depuis RFID) |

- Connectez l'oscillateur externe (20 MHz) aux pins OSC1/OSC2.
- Assurez-vous que l'alimentation et les masses sont correctement connectées.

## Prérequis logiciels
- **IDE** : MPLAB X IDE (dernière version recommandée)
- **Compilateur** : XC8 (compatible avec PIC16F877A)
- **Programmateur** : MPLAB ICD, PICkit 3/4, ou équivalent
- **Système d'exploitation** : Windows, macOS, ou Linux

## Installation
1. Clonez ce dépôt :
   ```bash
   git clone https://github.com/votre-nom/système-sécurité-rfid-pic16f877a.git

1. Ouvrir dans MPLAB X IDE
2. Compiler avec XC8
3. Programmer le PIC
##💾 Base de Données RFID
1. Modifier le tableau des tags autorisés:
   ```bash
   const char *authorized_tags[] = {
    "1A2B3C4D5E", // Tag admin
    "5F6G7H8I9J", // Tag utilisateur
    NULL // Fin de liste
   };

