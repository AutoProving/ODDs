// Copyright 2019 Daniel Notland
// This file is licensed under MIT License, as specified in the file LICENSE located at the root folder of this repository.

#include "odd.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void intersectionLayers(Layer* layer1, Layer* layer2, Layer* result);
void intersectionODD(ODD* odd1, ODD* odd2, ODD* odd);

int countIntersectionTransitions(Layer* layer1, Layer* layer2);
void intersectionTransitions(Layer* layer1, Layer* layer2, Layer* result);
void intersectionStates(StateContainer* s1, StateContainer* s2, StateContainer* result, int l2width);

/*
 * Adds the cartesian product of layer1 and layer2 stats to result state using:
 * (a,b) = a * b.width + b
 */
void intersectionStates(StateContainer* s1, StateContainer* s2, StateContainer* result, int l2width) {
    result->nStates = s1->nStates * s2->nStates;
    result->set = (State*) malloc(sizeof(State) * result->nStates);

    int index = 0;
    for(int i = 0; i < s1->nStates; i++) {
        for(int j = 0; j < s2->nStates; j++) {
            result->set[index++] = s1->set[i] * l2width + s2->set[j];
        }
    }
}

/*
 * Counts the number of transitions in layer1 and layer2 with same a
 */
int countIntersectionTransitions(Layer* layer1, Layer* layer2) {
    int nTrans = 0;
    for(int i = 0; i < layer1->transitions.nTransitions; i++ ) {
        for(int j = 0; j < layer2->transitions.nTransitions; j++) {
            if(layer1->transitions.set[i].a == layer2->transitions.set[i].a) {
                nTrans++;
            }
        }
    }
    return nTrans;
}

/*
 * Adds the transitions from layer1 and layer2 where l1.trans.a = l2.trans.a in result as
 * ((l1.s1, l2.s1), a, (l1.s2, l2.s2))
 */
void intersectionTransitions(Layer* layer1, Layer* layer2, Layer* result) {
    result->transitions.nTransitions = countIntersectionTransitions(layer1, layer2);
    result->transitions.set = (Transition*) malloc(sizeof(Transition) * result->transitions.nTransitions);

    int index = 0;
    for(int i = 0; i < layer1->transitions.nTransitions; i++ ) {
        for(int j = 0; j < layer2->transitions.nTransitions; j++) {
            if(layer1->transitions.set[i].a == layer2->transitions.set[i].a) {
                Transition transition;
                transition.s1 = layer1->transitions.set[i].s1 * layer2->width * layer2->transitions.set[j].s1;
                transition.s2 = layer1->transitions.set[i].s2 * layer2->width * layer2->transitions.set[j].s2;
                transition.a = layer1->transitions.set[i].a;
                result->transitions.set[index++] = transition;
            }
        }
    }
}

void intersectionLayers(Layer* layer1, Layer* layer2, Layer* result) {
    intersectionStates(&layer1->leftStates, &layer2->leftStates, &result->leftStates, layer2->width);
    intersectionStates(&layer1->rightStates, &layer2->rightStates, &result->rightStates, layer2->width);
    intersectionStates(&layer1->initialStates, &layer2->initialStates, &result->initialStates, layer2->width);
    intersectionStates(&layer1->finalStates, &layer2->finalStates, &result->finalStates, layer2->width);
    intersectionTransitions(layer1, layer2, result);
    result->initialFlag = layer1->initialFlag;
    result->finalFlag = layer1->finalFlag;
    result->map = layer1->map;
    result->width = result->leftStates.nStates > result->rightStates.nStates ? result->leftStates.nStates : result->rightStates.nStates;
}

void intersectionODD(ODD* odd1, ODD* odd2, ODD* odd) {
    assert(odd1->nLayers == odd2->nLayers);
    odd->nLayers = odd1->nLayers;
    odd->width = 0;
    odd->layerSequence = (Layer*) malloc(sizeof(Layer) * odd->nLayers);
    for(int i = 0; i < odd->nLayers; i++) {
        Layer layer;
        intersectionLayers(&odd1->layerSequence[i], &odd2->layerSequence[i], &layer);
        odd->layerSequence[i] = layer;
        if(layer.width > odd->width) {
            odd->width = layer.width;
        }
    }
}