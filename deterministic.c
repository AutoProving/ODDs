// Copyright 2019 Ole Magnus Morken
// This file is licensed under MIT License, as specified in the file LISENSE located at the root folder of this repository.

#include "odd.h"
#include <stdio.h>
#include <stdlib.h>

int isDeterministicLayerBool(Layer* layer);
int isDeterministicODDBool(ODD* odd);


void isDeterministicLayer(Layer* layer) {
    if (isDeterministicLayerBool(layer) == 1) {
        printf("Layer is deterministic.");
    }
    else {
        printf("Layer is NOT deterministic.");
    }
}

void isDeterministicODD(ODD* odd) {
    if (isDeterministicODDBool(odd) == 1) {
        printf("ODD is deterministic.");
    }
    else {
        printf("ODD is NOT deterministic.");
    }
}

//Return 0 if false, 1 if true.
//Assumes set of transitions is sorted.
int isDeterministicLayerBool(Layer* layer) {
    TransitionContainer trans = layer -> transitions;
    Transition* transition_set = trans.set;
    int num_transitions = trans.nTransitions;

    for(size_t i = 1; i < num_transitions; i++)
    {
        if (transition_set[i - 1].s1 == transition_set[i].s1) {
            if (transition_set[i - 1].a == transition_set[i].a) {
                return 0;
            }
        }
    }
    
    return 1;
}

int isDeterministicODDBool(ODD* odd) {
    int num_layers = odd -> nLayers;
    Layer* layers = odd -> layerSequence;

    for(size_t i = 0; i < num_layers; i++)
    {
        if (isDeterministicLayerBool(&layers[i]) == 0) {
            return 0;
        }
    }

    return 1;
}