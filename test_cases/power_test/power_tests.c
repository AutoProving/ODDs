// Copyright 2019 Markus Ragnhildstveit, Amund Lindberg, Ole Magnus Morken, Guillermo Hoyo Bravo and Josep Barbera Muñoz
// This file is licensed under MIT License, as specified in the file LISENSE located at the root folder of this repository.

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../../odd.h"

void showResult(ODD odd, ODD powerODD);
void showStates(StateContainer states);
void showTransitions(TransitionContainer transitions);

void run_powerset();
void run_powerlazy();

int main() {

    //compile:
    // gcc power_tests.c ../../powerSet.c ../../powerSetLazy.c ../../reading.c

    printf("Running...\n");

    //run_powerset();

    run_powerlazy();

    printf("\"Test\"(s) complete.\n");
    return 0;

}

void run_powerset() {
    ODD odd, power;
    readODD("ODD-example.txt", &odd);
    //readODD("ODD-example-power.txt", &odd);

    power = *powerSetODD(&odd);

    printf("\nResult of PowerSetODD():\n");
    showResult(odd, power);
    printf("\nDone printing the result of PowerSetODD().\n");
}


void run_powerlazy() {
    ODD odd, lazy;
    readODD("ODD-Example.txt", &odd);
    //readODD("ODD-example-power.txt", &odd);

    lazy = *lazy_power_ODD(&odd);

    printf("\nResult of lazy_power_ODD():\n");
    showResult(odd, lazy);
    printf("\nDone printing the result of lazy_power_ODD().\n");
}


void showResult(ODD odd, ODD result)
{
    printf("original odd has width: %d and %d layers.\n", odd.width, odd.nLayers);
    printf("result odd has width: %d and %d layers.\n", result.width, result.nLayers);

    for (int i = 0; i < result.nLayers; i++)
    {

        Layer pl = result.layerSequence[i];
        printf("\nLayer %d =\ninitial_flag: %d\nfinal_flag: %d\n", i, pl.initialFlag, pl.finalFlag);

        if (pl.initialStates.nStates > 0)
        {
            printf("initial_states: ");
            showStates(pl.initialStates);
        }

        printf("left_states: ");
        showStates(pl.leftStates);

        printf("right_states: ");
        showStates(pl.rightStates);

        if (pl.finalStates.nStates > 0)
        {
            printf("final_states: ");
            showStates(pl.finalStates);
        }

        printf("transitions: ");
        showTransitions(pl.transitions);

        //alphabetmap
    }
}



void showTransitions(TransitionContainer transitions)
{
    printf("{");
    for (int i = 0; i < transitions.nTransitions; i++)
    {
        printf("(%d, %d, %d)", transitions.set[i].s1, transitions.set[i].s2, transitions.set[i].a);
        if (i != transitions.nTransitions - 1)
        {
            printf(", ");
        }
    }
    printf("}\n");
}


void showStates(StateContainer states)
{
    printf("{");
    for (int i = 0; i < states.nStates - 1; i++)
    {
        printf("%d, ", states.set[i]);
    }
    printf("%d}\n", states.set[states.nStates - 1]);
}