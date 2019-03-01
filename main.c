#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "odd.h"

void testbitshift()
{
    int layerwidth = 2;
    int lwpow = 4;
    int *s = calloc(layerwidth, sizeof(int));

    for (int i = 0; i < lwpow; i++)
    {

        printf("content of S: ");
        for (int j = 0; j < layerwidth; j++)
        {
            printf("%d ", s[j]);
        }
        printf("\n");

        // Do the bit shifting
        for (size_t i = 0; i < layerwidth; i++)
        {
            if (s[i] != 1)
            {
                s[i] = 1;
                break;
            }
            else
            {
                s[i] = 0;
            }
        }
    }
}

void showTransitions(TransitionContainer transitions) {
    printf("{");
    for (int i = 0; i < transitions.nTransitions - 1; i++) {
        printf("%d, ", transitions.set[i]);
    }
    printf("%d}\n", transitions.set[transitions.nTransitions - 1]);
}

void showStates(StateContainer states) {
    printf("{");
    for (int i = 0; i < states.nStates - 1; i++) {
        printf("%d, ", states.set[i]);
    }
    printf("%d}\n", states.set[states.nStates-1]);
}

void testPowerSetODD(ODD odd, ODD powerODD)
{
    printf("\n------------------------ Result of PowerSetODD ---------------------------\n");
    printf("--------------------------------------------------------------------------\n");
    printf("Original ODD has width: %d and powerODD has width: %d\n", odd.width, powerODD.width);
    printf("Original ODD has %d layers and powerODD has %d layers\n", odd.nLayers, powerODD.nLayers);

    for (int i = 0; i < powerODD.nLayers; i++) {
        
        Layer pl = powerODD.layerSequence[i];
        printf("\nPowerLayer %d with initFlag = %d, finalFlag = %d\n", i, pl.initialFlag, pl.finalFlag);

        printf("InitialStates: ");
        showStates(pl.initialStates);

        printf("LeftStates: ");
        showStates(pl.leftStates); 

        printf("RightStates: ");
        showStates(pl.rightStates);

        printf("FinalStates: ");
        showStates(pl.finalStates);

        printf("Transitions: ");
        showTransitions(pl.transitions);

        //alphabetmap
    }
    
    printf("--------------------------------------------------------------------------\n");
    printf("--------------------------------------------------------------------------\n");
    printf("Done printing powerODD\n");
}


int main()
{
    // printf() displays the string inside quotation
    // printf("Hello, World!");

    ODD odd, powerODD;
    readODD("ODD-Example.txt", &odd);

    powerSetODD(&odd, &powerODD);

    testPowerSetODD(odd, powerODD);

    printODD("powerODDResult.txt", &powerODD);
    return 0;
}
