// Copyright 2019 Christian Stafset, Daniel Lansdaal, Konstantin _
// This file is licensed under MIT License, as specified in the file LISENSE located at the root folder of this repository.
#include <stdio.h>
#include <stdlib.h>
#include "odd.h"

void deleteDeadStates(Layer* layer, int* livingCount, int* livingStates) {
    //can compress these variable declarations
    StateContainer cont = layer->rightStates;
    State* states = cont.set;
    TransitionContainer transCont = layer->transitions;
    Transition* trans = transCont.set;
    int numTransitions = transCont.nTransitions;
    int numStates = cont.nStates;
    *livingCount = 0;
    //can make faster by running through only transitions
    //go through all transitions and add states on the right into new array make sure you dont double up
    //if you can ensure transitions come from a valid left state (which should be the case as we deleted dead states from the left)
    for (int i = 0; i < numStates; i++) {
        for (int j = 0; j < numTransitions; j++) {
            if (trans[j].s2 == states[i]) {
                livingStates[*livingCount] = states[i];
                *livingCount++;
                break;
            }
        }
    }
}

void deleteDeadTransitions(Layer* layer, int* livingCount, int* livingStates, int* livingTransCount, Transition* livingTransitions) {
    //compress variable declarations
    TransitionContainer transCont = layer->transitions;
    Transition* trans = transCont.set;
    int transCount = transCont.nTransitions;
    *livingTransCount = 0;
    //can do in logn time because sorted
    //binary search??
    for (int i = 0; i < livingCount[0]; i++) {
        for (int j = 0; j < transCount; j++) {
            if (trans[j].s1 == livingStates[i]) {
                livingTransitions[*livingTransCount] = trans[j];
                *livingTransCount++;
            }
        }
    }
}


void minimize (ODD* odd) {

    //create copy of ODD, I guess this is how we copy anything we need to 
    ODD* p = &odd;
    ODD newODD = *p;

    //better to create new ODD and add layers to it like we thought orignally LOL

    int numLayers = odd->nLayers;

    //create new layer then we need to free the old one before replacing it with the new one we created
    Layer l;
    //change things in the layer using '.' e.g.
    l.width = newODD.layerSequence[0].width;

    Layer* newLayer = &(odd->layerSequence[0]);
    int* livingStates = newLayer->leftStates.set;
    Transition* livingTransitions = newLayer->transitions.set;

    int livingCount = newLayer->leftStates.nStates;
    int livingTransCount = newLayer->transitions.nTransitions;

    for (int i = 0; i < numLayers; i++) {
        newLayer = &(odd->layerSequence[i]);
        newLayer->leftStates.set = livingStates;
        newLayer->leftStates.nStates = livingCount;
        deleteDeadTransitions(newLayer, &livingCount, livingStates, &livingTransCount, livingTransitions);
        deleteDeadStates(newLayer, &livingCount, livingStates);
        newLayer->rightStates.set = livingStates;
        newLayer->rightStates.nStates = livingCount;

        //memory leak don't free OLD transitions before setting the pointer to the NEW transitions
        newLayer->transitions.set = livingTransitions;
        newLayer->transitions.nTransitions = livingTransCount;

        newLayer->width = newLayer->leftStates.nStates > newLayer->rightStates.nStates ? newLayer->leftStates.nStates : newLayer->rightStates.nStates;

    }

    for(int i = 0; i < odd->nLayers; i++) {
        odd->width = odd->layerSequence[i].width > odd->width ? odd->layerSequence[i].width : odd->width;
    }
}