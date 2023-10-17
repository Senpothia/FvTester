/**
 TEST CARTE D855ED2
 * 
 * 
 * Michel 
 * 
 */

/*
 Equipement: automate v3.0
 * PIC16LF1939
 * 
 * Relais:
 * 
 * R1: alimentation
 * R2: BP1
 * R3: BP2
 * R4: Horloge
 * R5: Cavalier P1
 * R6: Cavalier P2
 * R7: Touche clavier - "Doigt"
 * R8: Avant séquence de test = indicateur inhibition tests des leds; dans sequence de test = indicateur éclairage clavier
 * 
 * Entrées:
 * 
 * J13/(IN1 dans le code): NC1
 * J15/IN2: NO1
 * J16/IN4: NC2
 * J17/IN5: NO2
 * J19/IN6: NC3
 * J18/IN7: NO3
 * J14/IN3: Bouton de validation OK 
 * J20/IN8: Bouton de confirmation NOK
 * J21/AN1: test éclairage clavier par mesure analogique
 * 
 * Sorties:
 * J25.1/C2: led rouge (indication non conforme)
 * J25.2/C4: led verte (indication conforme)
 * J25.3/C2: led jaune (indication test en cours)
 * J26(serigraphie OV)/GPIO1: inhibition test des leds - starp au OV = tests des leds inhibé
 * 
 * Mode d'emploi:
 * 1- Sélectionner mode de test des leds (strap = inhibition)
 * 2- Appuyer sur OK, pour lancer la séquence
 * 3- Appuyer sur OK pour valider / sur NOK pour invalider
 * 4- En fin de séquence le résultat est donné par la led verte ou rouge
 * 5- Acquiter le résultat en appuyant sur OK
 * 
 * 
 */

#include "mcc_generated_files/mcc.h"
#include "I2C_LCD.h"
#include "tester.h"
#include "display.h"
#include <string.h>

/*
                         Main application
 */
void main(void) {
    // initialize the device
    SYSTEM_Initialize();

    // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();


    // lCD
    I2C_Master_Init();
    LCD_Init(0x4E); // Initialize LCD module with I2C address = 0x4E
    bool testActif = false;
    bool testVoyants = false;
    bool automatique = false;
    char messageDefautCP[20];
    int cps = 0;
    bool erreurCPs = false;

    // Affichage message d'accueil

    displayManager(TITRE, BOARD_REQUEST, OK_REQUEST, LIGNE_VIDE);
    __delay_ms(1000);


    while (1) {

        // Test des positions de CP

        cps = testCP();

        if (cps != 0) {

            ledsAlerte();
            errorAlert();
            int tentatives = 0;
            while (tentatives < 2 && cps != 0) {

                switch (cps) {

                    case 1:

                        strncpy(messageDefautCP, DEFAUTCP1, 20);
                        break;

                    case 2:
                        strncpy(messageDefautCP, DEFAUTCP2, 20);
                        break;

                    case 3:
                        strncpy(messageDefautCP, DEFAUTCP3, 20);
                        break;

                }

                displayManager(TITRE, messageDefautCP, BOARD_REF, ACQ);
                attenteOK();
                debloquerCPs();
                cps = testCP();
                tentatives++;

            }

            if (cps != 0) {

                erreurCPs = true;
                errorAlert();
            }

        }

        while (erreurCPs) {

            displayManager(TITRE, messageDefautCP, "MAINTENANCE NECESSAIRE", "REDEMARREZ BANC");
        }

        if (!erreurCPs) {

            // Attente de démarrage

            displayManager(TITRE, ATTENTE, LIGNE_VIDE, OK_REQUEST);
            __delay_ms(100);

            attenteDemarrage(&automatique, &testActif);
            testActif = true;
            startAlert();
            displayManager("ETAPE 1", "TEST MODE BP", LIGNE_VIDE, LIGNE_VIDE);
            testActif = true;
            ledConforme(false);
            ledNonConforme(false);
            ledProgession(true);

            // entrée dans la séquence de test

            // ETAPE 1

            __delay_ms(100);

            modeBP(true);
            IN_OFF();
            activerBP(false);
            activerReed(false);
            __delay_ms(800);

            if (testFermeture(false)) {

            } else {

                testActif = false;

                alerteDefaut("ETAPE 1", &testActif, &testVoyants);
                sortieErreur(&automatique, &testActif, &testVoyants);

            }

            // ETAPE 2

            if (testActif) {

                displayManager("ETAPE 2", "MODE BP", LIGNE_VIDE, LIGNE_VIDE);
                IN12();
                activerBP(false);
                activerReed(true);

                __delay_ms(800);
                if (testFermeture(true)) {

                } else {

                    testActif = false;
                    alerteDefaut("ETAPE 2", &testActif, &testVoyants);
                    sortieErreur(&automatique, &testActif, &testVoyants);
                }
            }

            // ETAPE 3

            if (testActif) {

                displayManager("ETAPE 3", "MODE BP", LIGNE_VIDE, LIGNE_VIDE);
                IN12();
                activerBP(true);
                activerReed(false);

                __delay_ms(800);
                if (testFermeture(false)) {

                } else {

                    testActif = false;
                    alerteDefaut("ETAPE 3", &testActif, &testVoyants);
                    sortieErreur(&automatique, &testActif, &testVoyants);
                }
            }

            // ETAPE 4

            if (testActif) {

                displayManager("ETAPE 4", "MODE BP", LIGNE_VIDE, LIGNE_VIDE);
                IN_OFF();
                activerBP(false);
                activerReed(false);

                __delay_ms(800);
                if (testFermeture(false)) {

                } else {

                    testActif = false;
                    alerteDefaut("ETAPE 3", &testActif, &testVoyants);
                    sortieErreur(&automatique, &testActif, &testVoyants);
                }
            }


            // ETAPE 5

            if (testActif) {

                displayManager("ETAPE 5", "MODE BP", LIGNE_VIDE, LIGNE_VIDE);
                IN48();
                activerBP(false);
                activerReed(true);

                __delay_ms(800);
                if (testFermeture(true)) {

                } else {

                    testActif = false;
                    alerteDefaut("ETAPE 5", &testActif, &testVoyants);
                    sortieErreur(&automatique, &testActif, &testVoyants);
                }
            }

            // ETAPE 6

            if (testActif) {

                displayManager("ETAPE 6", "MODE BP", LIGNE_VIDE, LIGNE_VIDE);
                IN48();
                activerBP(true);
                activerReed(false);

                __delay_ms(800);
                if (testFermeture(false)) {

                } else {

                    testActif = false;
                    alerteDefaut("ETAPE 6", &testActif, &testVoyants);
                    sortieErreur(&automatique, &testActif, &testVoyants);
                }
            }

            // ETAPE 7

            if (testActif) {

                displayManager("ETAPE 7", "MODE BP", LIGNE_VIDE, LIGNE_VIDE);
                IN_OFF();
                activerBP(false);
                activerReed(false);

                __delay_ms(800);
                if (testFermeture(false)) {

                } else {

                    testActif = false;
                    alerteDefaut("ETAPE 7", &testActif, &testVoyants);
                    sortieErreur(&automatique, &testActif, &testVoyants);
                }
            }

            // ETAPE 8

            if (testActif) {

                displayManager("ETAPE 8", "MODE TC", LIGNE_VIDE, LIGNE_VIDE);
                modeBP(false);
                IN_OFF();
                TC_OFF();
                activerReed(false);

                __delay_ms(800);
                if (testFermeture(false)) {

                } else {

                    testActif = false;
                    alerteDefaut("ETAPE 8", &testActif, &testVoyants);
                    sortieErreur(&automatique, &testActif, &testVoyants);
                }
            }


            // ETAPE 9

            if (testActif) {

                displayManager("ETAPE 9", "MODE TC", LIGNE_VIDE, LIGNE_VIDE);
                IN12();
                TC12();
                activerReed(true);

                __delay_ms(800);
                if (testFermeture(true)) {

                } else {

                    testActif = false;
                    alerteDefaut("ETAPE 9", &testActif, &testVoyants);
                    sortieErreur(&automatique, &testActif, &testVoyants);
                }
            }

            // ETAPE 10

            if (testActif) {

                displayManager("ETAPE 10", "MODE TC", LIGNE_VIDE, LIGNE_VIDE);
                IN12();
                TC_OFF();
                activerReed(false);

                __delay_ms(800);
                if (testFermeture(false)) {

                } else {

                    testActif = false;
                    alerteDefaut("ETAPE 10", &testActif, &testVoyants);
                    sortieErreur(&automatique, &testActif, &testVoyants);
                }
            }

            // ETAPE 11

            if (testActif) {

                displayManager("ETAPE 11", "MODE TC", LIGNE_VIDE, LIGNE_VIDE);
                IN_OFF();
                TC_OFF();
                activerReed(false);

                __delay_ms(800);
                if (testFermeture(false)) {

                } else {

                    testActif = false;
                    alerteDefaut("ETAPE 11", &testActif, &testVoyants);
                    sortieErreur(&automatique, &testActif, &testVoyants);
                }
            }

            // ETAPE 12

            if (testActif) {

                displayManager("ETAPE 12", "MODE TC", LIGNE_VIDE, LIGNE_VIDE);
                IN48();
                TC_OFF();
                activerReed(false);

                __delay_ms(800);
                if (testFermeture(false)) {

                } else {

                    testActif = false;
                    alerteDefaut("ETAPE 12", &testActif, &testVoyants);
                    sortieErreur(&automatique, &testActif, &testVoyants);
                }
            }

            // ETAPE 13

            if (testActif) {

                displayManager("ETAPE 13", "MODE TC", LIGNE_VIDE, LIGNE_VIDE);
                IN48();
                TC48();
                activerReed(true);

                __delay_ms(800);
                if (testFermeture(true)) {

                } else {

                    testActif = false;
                    alerteDefaut("ETAPE 13", &testActif, &testVoyants);
                    sortieErreur(&automatique, &testActif, &testVoyants);
                }
            }

            // ETAPE 14

            if (testActif) {

                displayManager("ETAPE 14", "MODE TC", LIGNE_VIDE, LIGNE_VIDE);
                IN48();
                TC_OFF();
                activerReed(false);

                __delay_ms(800);
                if (testFermeture(false)) {

                } else {

                    testActif = false;
                    alerteDefaut("ETAPE 14", &testActif, &testVoyants);
                    sortieErreur(&automatique, &testActif, &testVoyants);
                }
            }


            // ETAPE 15

            if (testActif) {

                displayManager("ETAPE 15", "MODE TC", LIGNE_VIDE, LIGNE_VIDE);
                IN_OFF();
                TC48();
                activerReed(false);

                __delay_ms(800);
                if (testFermeture(false)) {

                } else {

                    testActif = false;
                    alerteDefaut("ETAPE 15", &testActif, &testVoyants);
                    sortieErreur(&automatique, &testActif, &testVoyants);
                }
            }

            // ETAPE 16

            if (testActif) {

                displayManager("ETAPE 16", "TEST LEDS VERTES", LIGNE_VIDE, ANSWER);
                modeBP(true);
                IN48();
                activerBP(true);
                activerReed(false);
                __delay_ms(800);

                printf("Attente validation led vertes\r\n");
                testVoyants = reponseOperateur(automatique);
                if (!testVoyants) {

                    testActif = false;
                    alerteDefaut("ETAPE 3", &testActif, &testVoyants);
                    sortieErreur(&automatique, &testActif, &testVoyants);
                }

            }

            // ETAPE 17

            if (testActif) {

                displayManager("ETAPE 17", "TEST LEDS BLEUES", LIGNE_VIDE, ANSWER);
                IN48();
                activerBP(true);
                activerReed(true);
                __delay_ms(800);

                printf("Attente validation leds bleues\r\n");
                testVoyants = reponseOperateur(automatique);
                if (!testVoyants) {

                    testActif = false;
                    alerteDefaut("ETAPE 17", &testActif, &testVoyants);
                    sortieErreur(&automatique, &testActif, &testVoyants);
                }

            }

            // ETAPE: SORTIE

            if (testActif) {

                displayManager("FIN DE TEST", "CONFORME", "RETIRER CARTE", ACQ);
                ledConforme(true);
                rasAlimention();
                okAlert();
                attenteAquittement(&automatique, &testActif);
                initialConditions(&testActif, &testVoyants, &automatique);
                cps = 0;
                __delay_ms(2000);

            }

        }


    } // fin boucle infinie



} // fin fonction main

/**
 End of File
 */