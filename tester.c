/*
 * File:   tester.c
 * Author: Michel
 *
 */


#include <xc.h>
#include "mcc_generated_files/mcc.h"
#include "I2C_LCD.h"
#include "tester.h"
#include "display.h"
#include <stdio.h>
#include <string.h>

bool testOK(bool active) {

    bool result = false;
    if (active) {

        if (IN3_GetValue() == 0) {

            result = true;
        }
    }

    if (!active) {

        if (IN3_GetValue() == 1) {

            result = true;
        }
    }

    return result;

}

bool testNOK(bool active) {

    bool result = false;
    if (active) {

        if (IN8_GetValue() == 0) {

            result = true;
        }
    }

    if (!active) {

        if (IN8_GetValue() == 1) {

            result = true;
        }
    }

    return result;
}

void ledNonConforme(bool active) {

    if (active) {

        C3_SetHigh();
    } else {
        C3_SetLow();
    }
}

void ledConforme(bool active) {

    if (active) {

        C4_SetHigh();
    } else {
        C4_SetLow();
    }
}

void ledProgession(bool active) {

    if (active) {

        C2_SetHigh();

    } else {

        C2_SetLow();
    }
}

void alerteDefaut(char etape[], bool *testAct, bool *testVoy) {

    char error[20] = "-> ERREUR: ";
    char eol[10] = "\r\n";
    ledNonConforme(true);
    ledProgession(true);
    ledConforme(false);
    rasAlimention();
    displayManager(etape, NON_CONFORME, ACQ, LIGNE_VIDE);
    printf(strcat(strcat(error, etape), eol));
    errorAlert();

    while (IN3_GetValue() == 0) {
        ;
    }

    *testAct = false;
    *testVoy = false;

}

bool reponseOperateur(bool automatique) {

    bool reponse = false;
    bool repOperateur = false;
    unsigned char reception;

    if (automatique) {

        while (!repOperateur) {

            if (eusartRxCount != 0) {

                reception = EUSART_Read(); // read a byte for RX

                switch (reception) // check command  
                {
                    case '2':
                    {

                        __delay_ms(50);
                        reponse = true;
                        repOperateur = true;
                        break;
                    }

                    case '3':
                    {

                        __delay_ms(50);
                        reponse = false;
                        repOperateur = true;
                        break;
                    }
                }

            }

        }

    }

    if (!automatique) {

        while (!repOperateur) {

            if (testNOK(true)) {
                reponse = false;
                repOperateur = true;
            }
            if (testOK(true)) {
                reponse = true;
                repOperateur = true;
            }
        }

    }

    return reponse;

}

void initialConditions(bool *testAct, bool *testVoy, bool *autom) {

    if (!*autom) {

        printf("-> FIN TEST MANUEL\r\n");
    }
    *testAct = false;
    *testVoy = false;
    *autom = false;
    ledConforme(false);
    ledNonConforme(false);
    ledProgession(false);
    rasAlimention();
}

void activerBuzzer() {


    for (int i = 0; i < 50; i++) {

        BUZ_SetHigh();
        __delay_us(1000);

        BUZ_SetLow();
        __delay_us(1000);

    }

}

void startAlert(void) {

    for (int i = 0; i < 2; i++) {

        activerBuzzer();
        __delay_ms(500);

    }

}

void errorAlert(void) {

    for (int j = 0; j < 4; j++) {

        for (int i = 0; i < 4; i++) {

            activerBuzzer();
            __delay_ms(500);

        }
        __delay_ms(500);
    }


}

void okAlert(void) {


    printf("-> TEST CONFORME - ATTENTE ACQUITTEMENT\r\n");
    for (int i = 0; i < 2; i++) {

        startAlert();

    }


}

void attenteDemarrage(bool *autom, bool *testAct) {

    unsigned char reception;
    bool repOperateur = false;

    while (!repOperateur) {


        if (IN3_GetValue() == 0) {

            printf("-> TEST MANUEL EN COURS\r\n");
            repOperateur = true;
            *autom = false;
        }

        if (eusartRxCount != 0) {

            reception = EUSART_Read(); // read a byte for RX

            switch (reception) // check command  
            {
                case '1':
                {
                    printf("-> TEST ON\r\n");
                    *autom = true;
                    __delay_ms(50);
                    repOperateur = true;
                    break;
                }
            }
        }
    }

}

void attenteAquittement(bool *autom, bool *testAct) {

    unsigned char reception;
    bool repOperateur = false;

    while (!repOperateur) {


        if (IN3_GetValue() == 0) {

            printf("-> FIN TEST MANUEL\r\n");
            repOperateur = true;
            *autom = false;
            *testAct = false;
        }

        if (eusartRxCount != 0) {

            reception = EUSART_Read(); // read a byte for RX

            switch (reception) // check command  
            {
                case '4':
                {
                    printf("-> TEST ACQUITTE\r\n");
                    *autom = false;
                    *testAct = false;
                    __delay_ms(50);
                    repOperateur = true;
                    break;
                }
            }
        }
    }

}

void sortieErreur(bool *autom, bool *testAct, bool *testVoy) {

    attenteAquittement(*autom, *testAct);
    initialConditions(*testAct, *testVoy, *autom);
    __delay_ms(2000);

}

// Fonctions particulières

bool testRelais(bool active) {

    bool result = false;
    if (active) {

        if (IN4_GetValue() == 0 && IN5_GetValue() == 1) {

            result = true;
        }
    }

    if (!active) {

        if (IN4_GetValue() == 1 && IN5_GetValue() == 0) {


            result = true;
        }
    }

    return result;
}

void rasAlimention() {

    REL1_SetLow();
    REL2_SetLow();
    REL3_SetLow();
    REL4_SetLow();
    REL5_SetLow();

}

void IN12() {

    REL2_SetHigh();
    REL1_SetLow();

}

void IN48() {

    REL1_SetHigh();
    REL2_SetLow();

}

void TC12() {

    REL5_SetHigh();
    REL4_SetLow();

}

void TC48() {

    REL5_SetLow();
    REL4_SetHigh();

}

void IN_OFF() {

    REL2_SetLow();
    REL1_SetLow();
}

void TC_OFF() {

    REL5_SetLow();
    REL4_SetLow();

}

void modeBP(bool active) {

    if (active) {

        REL8_SetLow();
        REL6_SetHigh();

    } else {

        REL8_SetHigh();
        REL6_SetLow();

    }

}

bool testFermeture(bool active) {


    bool result = false;
    if (active) {

        if (IN1_GetValue() == 1 && IN2_GetValue() == 1) {

            result = true;
        }
    }

    if (!active) {

        if (IN1_GetValue() == 0 && IN2_GetValue() == 0) {


            result = true;
        }
    }

    return result;

}

void activerReed(bool active) {

    if (active) {

        REL7_SetHigh();

    } else {

        REL7_SetLow();
    }

}

void activerBP(bool active) {

    if (active) {

        REL3_SetHigh();

    } else {

        REL3_SetLow();
    }

}

int testCP() {

    int result = 0;

    if (IN1_GetValue() == 1) {

        result = 1;
    }

    if (IN2_GetValue() == 1) {

        result = 2;
    }

    if (IN1_GetValue() == 1 && IN2_GetValue() == 1) {

        result = 3;
    }

    return result;
}

void debloquerCPs(void) {

    modeBP(true);
    IN48();
    __delay_ms(500);
    activerBP(true);
    __delay_ms(500);
    activerBP(true);
    IN_OFF();

}

void attenteOK(void) {

    while (IN3_GetValue() == 1) {
        ;
    }
}

