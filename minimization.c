// Copyright 2019 Christian Stafset, Daniel Lansdaal, Konstantin Rygol
// This file is licensed under MIT License, as specified in the file LISENSE located at the root folder of this repository.
#include <stdio.h>
#include <stdlib.h>
#include "odd.h"

Layer deleteDeadStatesLeft(Layer* layer, int* livingCount, int* livingStates, Layer l) {
    Transition* transitions = layer->transitions.set;
    int numTransitions = layer->transitions.nTransitions;
    int numStates = layer->leftStates.nStates;
    State * states = layer->leftStates.set;
    *livingCount = 0;
    //can make faster by running through only transitions
    //go through all transitions and add states on the right into new array make sure you dont double up
    //if you can ensure transitions come from a valid left state (which should be the case as we deleted dead states from the left)

    //good idea but how do we avoid getting double ups?

    
    for (int i = 0; i < numStates; i++) {
        for (int j = 0; j < numTransitions; j++) {
            if (transitions[j].s2 == states[i]) {
                livingStates[*livingCount] = states[i];
                *livingCount++;
                break;
            }
        }
    }

    l.leftStates.set = livingStates;
    l.leftStates.nStates = livingCount;
    return l;
    
}

Layer deleteDeadTransitionsLeft(Layer* layer, int livingCount, int* livingStates, int* livingTransCount, Transition* livingTransitions, Layer l) {
    Transition* transitions = layer->transitions.set;
    int transCount = layer->transitions.nTransitions;
    *livingTransCount = 0;
    //can do in n*logn time because sorted
    //binary search??
    for (int i = 0; i < livingCount; i++) {
        for (int j = 0; j < transCount; j++) {
            if (transitions[j].s1 == livingStates[i]) {
                livingTransitions[*livingTransCount] = transitions[j];
                *livingTransCount++;
            }
        }
    }

    l.leftStates.set = livingTransitions;
    l.transitions.nTransitions = livingTransCount;
}

Layer deleteDeadStatesRight(Layer* layer, int* livingCount, int* livingStates, Layer l) {
    Transition* transitions = layer->transitions.set;
    int numTransitions = layer->transitions.nTransitions;
    int numStates = layer->rightStates.nStates;
    State * states = layer->rightStates.set;
    *livingCount = 0;
    //can make faster by running through only transitions
    //go through all transitions and add states on the right into new array make sure you dont double up
    //if you can ensure transitions come from a valid left state (which should be the case as we deleted dead states from the left)

    //good idea but how do we avoid getting double ups?

    
    for (int i = 0; i < numStates; i++) {
        for (int j = 0; j < numTransitions; j++) {
            if (transitions[j].s2 == states[i]) {
                livingStates[*livingCount] = states[i];
                *livingCount++;
                break;
            }
        }
    }

    l.rightStates.set = livingStates;
    l.rightStates.nStates = livingCount;
    return l;
    
}

Layer deleteDeadTransitionsRight(Layer* layer, int livingCount, int* livingStates, int* livingTransCount, Transition* livingTransitions, Layer l) {
    Transition* transitions = layer->transitions.set;
    int transCount = layer->transitions.nTransitions;
    *livingTransCount = 0;
    //can do in n*logn time because sorted
    //binary search??
    for (int i = 0; i < livingCount; i++) {
        for (int j = 0; j < transCount; j++) {
            if (transitions[j].s1 == livingStates[i]) {
                livingTransitions[*livingTransCount] = transitions[j];
                *livingTransCount++;
            }
        }
    }

    l.transitions.set = livingTransitions;
    l.transitions.nTransitions = livingTransCount;
}

//need to go twice, forwards and backwards to remove states which have nothing from the left and nothing from the right

ODD* minimize (ODD* odd) {

    // //create copy of ODD, I guess this is how we copy anything we need to 
    // ODD* p = &odd;
    // ODD newODD = *p;

    //maybe this is better to define new ODD
    ODD o;
    o.nLayers = odd->nLayers;

    o.layerSequence = malloc(o.nLayers * sizeof(Layer));

    //better to create new ODD and add layers to it like we thought orignally LOL

    int numLayers = odd->nLayers;

    //create new layer then we need to free the old one before replacing it with the new one we created
    Layer l;
    //change things in the layer using '.' e.g.
    l.width = odd->layerSequence[0].width;

    Layer currLayer = odd->layerSequence[0];

    State* livingStates = currLayer.leftStates.set;
    int livingCount = currLayer.leftStates.nStates;

    Transition* livingTransitions =  currLayer.transitions.set;
    int livingTransCount = currLayer.transitions.nTransitions;

    for (int i = 0; i < numLayers; i++) {
        l.leftStates.set = livingStates;
        l.leftStates.nStates = livingCount;
        l = deleteDeadTransitionsRight(&currLayer, &livingCount, livingStates, &livingTransCount, livingTransitions, l);
        l = deleteDeadStatesRight(&currLayer, &livingCount, livingStates, l);

        l.width = l.leftStates.nStates > l.rightStates.nStates ? l.leftStates.nStates : l.rightStates.nStates;

        o.layerSequence[i] = l;
    }

    for (int i = numLayers-1; i >= 0; i--) {
        l.rightStates.set = livingStates;
        l.rightStates.nStates = livingCount;
        l = deleteDeadTransitionsLeft(&currLayer, &livingCount, livingStates, &livingTransCount, livingTransitions, l);
        l = deleteDeadStatesLeft(&currLayer, &livingCount, livingStates, l);

        l.width = l.leftStates.nStates > l.rightStates.nStates ? l.leftStates.nStates : l.rightStates.nStates;

        o.layerSequence[i] = l;
    }

    for(int i = 0; i < odd->nLayers; i++) {
        odd->width = odd->layerSequence[i].width > odd->width ? odd->layerSequence[i].width : odd->width;
    }

    //free original ODD?
    free(odd);

    return &o;
}