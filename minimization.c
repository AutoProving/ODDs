// Copyright 2019 YOUR NAME
// This file is licensed under MIT License, as specified in the file LISENSE located at the root folder of this repository.
#include <stdio.h>
#include <stdlib.h>
#include "odd.h"

void deleteDeadStates(Layer* layer, int* livingCount, int* livingStates) {
    StateContainer cont = layer->rightStates;
    State* states = cont.set;
    TransitionContainer transCont = layer->transitions;
    Transition* trans = transCont.set;
    int numTransitions = transCont.nTransitions;
    int numStates = cont.nStates;
    int i;
    int j;
    livingCount[0] = 0;
    for (i = 0; i < numStates; i++) {
        for (j = 0; j < numTransitions; j++) {
            if (trans[j].s2 == states[i]) {
                livingStates[livingCount[0]] = states[i];
                livingCount[0]++;
                break;
            }
        }
    }
}

void deleteDeadTransitions(Layer* layer, int* livingCount, int* livingStates, int* livingTransCount, Transition* livingTransitions) {
    TransitionContainer transCont = layer->transitions;
    Transition* trans = transCont.set;
    int transCount = transCont.nTransitions;
    livingTransCount[0] = 0;
    int i;
    int j;
    for (i = 0; i < livingCount[0]; i++) {
        for (j = 0; j < transCount; j++) {
            if (trans[j].s1 == livingStates[i]) {
                livingTransitions[livingTransCount[0]] = trans[j];
                livingTransCount[0]++;
            }
        }
    }
}


void minimize (ODD* odd) {
    int i;

    int numLayers = odd->nLayers;
    Layer* newLayer = &(odd->layerSequence[0]);
    int* livingStates = newLayer->leftStates.set;
    Transition* livingTransitions = newLayer->transitions.set;

    int* livingCount;
    livingCount[0] = newLayer->leftStates.nStates;
    int* livingTransCount;
    livingTransCount[0] = newLayer->transitions.nTransitions;

    for (i = 0; i < numLayers; i++) {
        newLayer = &(odd->layerSequence[i]);
        newLayer->leftStates.set = livingStates;
        newLayer->leftStates.nStates = livingCount[0];
        deleteDeadTransitions(newLayer, livingCount, livingStates, livingTransCount, livingTransitions);
        deleteDeadStates(newLayer, livingCount, livingStates);
        newLayer->rightStates.set = livingStates;
        newLayer->rightStates.nStates = livingCount[0];

        newLayer->transitions.set = livingTransitions;
        newLayer->transitions.nTransitions = livingTransCount[0];

        newLayer->width = newLayer->leftStates.nStates > newLayer->rightStates.nStates ? newLayer->leftStates.nStates : newLayer->rightStates.nStates;

    }

    for(int i = 0; i < odd->nLayers; i++) {
        odd->width = odd->layerSequence[i].width > odd->width ? odd->layerSequence[i].width : odd->width;
    }
}