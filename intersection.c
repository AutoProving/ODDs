// Copyright 2019 Daniel Notland
// This file is licensed under MIT License, as specified in the file LICENSE located at the root folder of this repository.

#include "odd.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void intersectionLayers(Layer* layer1, Layer* layer2, Layer* result);
void intersectionODD(ODD* odd1, ODD* odd2, ODD* odd);

void intersectionLeftStates(Layer* layer1, Layer* layer2, Layer* result);
void intersectionRightStates(Layer* layer1, Layer* layer2, Layer* result);
void intersectionInitialStates(Layer* layer1, Layer* layer2, Layer* result);
void intersectionFinalStates(Layer* layer1, Layer* layer2, Layer* result);
int countIntersectionTransitions(Layer* layer1, Layer* layer2);
void intersectionTransitions(Layer* layer1, Layer* layer2, Layer* result);

/*
 * Adds the cartesian product of layer1 and layer2 leftStates to result leftStates using:
 * (a,b) = a * b.width + b
 */
void intersectionLeftStates(Layer* layer1, Layer* layer2, Layer* result) {
    result->leftStates.nStates = layer1->leftStates.nStates * layer2->leftStates.nStates;
    result->leftStates.set = (State*) malloc(sizeof(State) * result->leftStates.nStates);

    int index = 0;
    for(int i = 0; i < layer1->leftStates.nStates; i++) {
        for(int j = 0; j < layer2->leftStates.nStates; j++) {
            result->leftStates.set[index++] = layer1->leftStates.set[i] * layer2->width + layer2->leftStates.set[j];
        }
    }
}

/*
 * Adds the cartesian product of layer1 and layer2 rightStates to result rightStates using:
 * (a,b) = a * b.width + b
 */
void intersectionRightStates(Layer* layer1, Layer* layer2, Layer* result) {
    result->rightStates.nStates = layer1->rightStates.nStates * layer2->rightStates.nStates;
    result->rightStates.set = (State*) malloc(sizeof(State) * result->rightStates.nStates);

    int index = 0;
    for(int i = 0; i < layer1->rightStates.nStates; i++) {
        for(int j = 0; j < layer2->rightStates.nStates; j++) {
            result->rightStates.set[index++] = layer1->rightStates.set[i] * layer2->width + layer2->rightStates.set[j];
        }
    }
}

/*
 * Adds the cartesian product of layer1 and layer2 initialStates to result initialStates using:
 * (a,b) = a * b.width + b
 */
void intersectionInitialStates(Layer* layer1, Layer* layer2, Layer* result) {
    result->initialStates.nStates = layer1->initialStates.nStates * layer2->initialStates.nStates;
    result->initialStates.set = (State*) malloc(sizeof(State) * result->initialStates.nStates);

    int index = 0;
    for(int i = 0; i < layer1->initialStates.nStates; i++) {
        for(int j = 0; j < layer2->initialStates.nStates; j++) {
            result->initialStates.set[index++] = layer1->initialStates.set[i] * layer2->width + layer2->initialStates.set[j];
        }
    }
}

/*
 * Adds the cartesian product of layer1 and layer2 finalStates to result finalStates using:
 * (a,b) = a * b.width + b
 */
void intersectionFinalStates(Layer* layer1, Layer* layer2, Layer* result) {
    result->finalStates.nStates = layer1->finalStates.nStates * layer2->finalStates.nStates;
    result->finalStates.set = (State*) malloc(sizeof(State) * result->finalStates.nStates);

    int index = 0;
    for(int i = 0; i < layer1->finalStates.nStates; i++) {
        for(int j = 0; j < layer2->finalStates.nStates; j++) {
            result->finalStates.set[index++] = layer1->finalStates.set[i] * layer2->width + layer2->finalStates.set[j];
        }
    }
};

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
    intersectionLeftStates(layer1, layer2, result);
    intersectionRightStates(layer1, layer2, result);
    intersectionInitialStates(layer1, layer2, result);
    intersectionFinalStates(layer1, layer2, result);
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