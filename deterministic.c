#include "odd.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <omp.h>

//Assumes set of transitions is sorted.
bool isDeterministicLayer(Layer* layer) {
    TransitionContainer trans = layer -> transitions;
    Transition* transition_set = trans.set;
    int num_transitions = trans.nTransitions;
    bool isDeterministic = true;

#pragma omp parallel for
    for(int i = 1; i < num_transitions; i++)
    {
        if (transition_set[i - 1].s1 == transition_set[i].s1) {
            if (transition_set[i - 1].a == transition_set[i].a) {
                isDeterministic = false;
            }
        }
    }
    
    return isDeterministic;
}

bool isDeterministicODD(ODD* odd) {
    int num_layers = odd -> nLayers;
    Layer* layers = odd -> layerSequence;
    bool isDeterministic = true;

#pragma omp parallel for
    for(int i = 0; i < num_layers; i++)
    {
        if (!isDeterministicLayer(&layers[i])) {
            isDeterministic = false;
        }
    }

    return isDeterministic;
}

