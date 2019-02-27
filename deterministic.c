// Copyright 2019 Ole Magnus Morken
// This file is licensed under MIT License, as specified in the file LISENSE located at the root folder of this repository.

#include "odd.h"
#include <stdio.h>
#include <stdlib.h>

//Return 0 if false, 1 if true.
//Assumes set of transitions is sorted.
int isDeterministicLayer(Layer* layer) {
    TransitionContainer trans = layer -> transitions;
    Transition* transition_set = trans.set;
    int num_transitions = trans.nTransitions;

    int width = layer -> width;

    if (width > num_transitions) {
        return 0;
    }

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

int isDeterministicODD(ODD* odd) {
    int num_layers = odd -> nLayers;
    Layer* layers = odd -> layerSequence;

    for(size_t i = 0; i < num_layers; i++)
    {
        if (isDeterministicLayer(&layers[i]) == 0) {
            //printf("Is not deterministic.");
            return 0;
        }
    }
    
    //printf("Is deterministic");

    return 1;
}