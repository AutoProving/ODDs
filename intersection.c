// Copyright 2019 Daniel Notland
// This file is licensed under MIT License, as specified in the file LICENSE located at the root folder of this repository.

#include "odd.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

void intersectionLayers(Layer* layer1, Layer* layer2, Layer* result);
void intersectionODD(ODD* odd1, ODD* odd2, ODD* odd);

int countIntersectionTransitions(Layer* layer1, Layer* layer2);
void intersectionTransitions(Layer* layer1, Layer* layer2, Layer* result);
void intersectionStates(StateContainer* s1, StateContainer* s2, StateContainer* result, int l2Width);

/*
 * Adds the cartesian product of layer1 and layer2 stats to result state using:
 * (a,b) = a * b.width + b
 */
void intersectionStates(StateContainer* s1, StateContainer* s2, StateContainer* result, int l2Width) {
    result->nStates = s1->nStates * s2->nStates;
    result->set = (State*) malloc(sizeof(State) * result->nStates);

    int index = 0;
    for(int i = 0; i < s1->nStates; i++) {
        for(int i1 = 0; i1 < s2->nStates; i1++) {
            result->set[index++] = s1->set[i] * l2Width + s2->set[i1];
        }
    }
}

/*
 * Adds the transitions from layer1 and layer2 where l1.trans.a = l2.trans.a in result as
 * ((l1.s1, l2.s1), a, (l1.s2, l2.s2))
 */
void intersectionTransitions(Layer* layer1, Layer* layer2, Layer* result) {
    result->transitions.set = (Transition*) malloc(sizeof(Transition) * layer1->transitions.nTransitions * layer2->transitions.nTransitions);

    int index = 0;
    for(int i = 0; i < layer1->transitions.nTransitions; i++ ) {
        for(int i1 = 0; i1 < layer2->transitions.nTransitions; i1++) {
            if(layer1->transitions.set[i].a == layer2->transitions.set[i].a) {
                result->transitions.set[index].s1 = layer1->transitions.set[i].s1 * layer2->width + layer2->transitions.set[i1].s1; // TODO: fix coordinate
                result->transitions.set[index].s2 = layer1->transitions.set[i].s2 * layer2->width + layer2->transitions.set[i1].s2; // TODO: fix coordinate
                result->transitions.set[index++].a = layer1->transitions.set[i].a;
            }
        }
    }
    result->transitions.nTransitions = index;
    //TODO: Is this the right way to reallocate memory?
    result->transitions.set = (Transition*) realloc(result->transitions.set, result->transitions.nTransitions * sizeof(Transition));
    assert(result->transitions.set != NULL);
}

void intersectionLayers(Layer* layer1, Layer* layer2, Layer* result) {
    intersectionStates(&layer1->leftStates, &layer2->leftStates, &result->leftStates, layer2->width);
    intersectionStates(&layer1->rightStates, &layer2->rightStates, &result->rightStates, layer2->width);
    intersectionStates(&layer1->initialStates, &layer2->initialStates, &result->initialStates, layer2->width);
    intersectionStates(&layer1->finalStates, &layer2->finalStates, &result->finalStates, layer2->width);
    intersectionTransitions(layer1, layer2, result);

    assert(layer1->initialFlag == layer2->initialFlag);
    assert(layer1->finalFlag == layer2->finalFlag);
    result->initialFlag = layer1->initialFlag;
    result->finalFlag = layer1->finalFlag;
    memcpy(&result->map, &layer1->map, sizeof(AlphabetMap));
    result->width = result->leftStates.nStates > result->rightStates.nStates ? result->leftStates.nStates : result->rightStates.nStates;
}

void intersectionODD(ODD* odd1, ODD* odd2, ODD* odd) {
    assert(odd1->nLayers == odd2->nLayers);
    odd->nLayers = odd1->nLayers;
    odd->width = 0;
    odd->layerSequence = (Layer*) malloc(sizeof(Layer) * odd->nLayers);
    for(int i = 0; i < odd->nLayers; i++) {
        intersectionLayers(&odd1->layerSequence[i], &odd2->layerSequence[i], &odd->layerSequence[i]);
        if(odd->layerSequence[i].width > odd->width) {
            odd->width = odd->layerSequence[i].width;
        }
    }
}