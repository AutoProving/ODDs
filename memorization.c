// Copyright 2019 Andreas Ommundsen
// This file is licensed under MIT License, as specified in the file LICENSE located at the root folder of this repository.

#include "odd.h"
#include <stdlib.h>

/*
 * Input:
 *      *layer : The layer whose number of states we are interested in.
 *      left : True for the left side StateContainer of the layer, false for the right side StateContainer.
 * Returns:
 *      The the new size of the given stateContainer = i.side * |S|
 * Where:
 *      i.side : the number of states on the given side.
 *      |S| : The size of the layer's alphabet.
 */
int expandedSetSize(Layer *layer, bool left);

/*
 * Input:
 *      *states : The StateContainer to be updated.
 *      newSize : The new size of the state set.
 * Effect:
 *      Memory of *states is freed and then a new array holding states [0 ... newSize-1] is set to that memory.
 */
void updateStateContainer(StateContainer *states, int newSize);

/*
 * Input:
 *      *states : Either the layer.initialStates or layer.finalStates to be updated.
 *      alphaSize : The layer.map.sizeAlphabet.
 * Effect:
 *      *states should be updated to match the expanded state numbers.
 */
void updateInitialFinal(StateContainer *states, int alphaSize);

/*
 * Input:
 *      *transitions: The layer.transitions TransitionsContainer that we wish to update.
 *      alphaSize : The layer.map.sizeAlphabet.
 * Effect:
 *      *transitions should be updated to show the mappings from the expanded left states to the expanded right states.
 */
void updateTransitions(TransitionContainer *transitions, int alphaSize);

Layer *memorizeLayer(Layer *layer) {

    Layer clonedLayer = *layer;
    Layer *cP = &clonedLayer;

    int newLeftSize = expandedSetSize(layer, true);
    int newRightSize = expandedSetSize(layer, false);
    int alphaSize = clonedLayer.map.sizeAlphabet;

    clonedLayer.width = (newLeftSize > newRightSize) ? newLeftSize : newRightSize;

    clonedLayer.leftStates.nStates = newLeftSize;
    updateStateContainer(&clonedLayer.leftStates, newLeftSize);

    clonedLayer.rightStates.nStates = newRightSize;
    updateStateContainer(&clonedLayer.rightStates, newRightSize);

    if (clonedLayer.initialFlag)
        updateInitialFinal(&clonedLayer.initialStates, alphaSize);

    if (clonedLayer.finalFlag)
        updateInitialFinal(&clonedLayer.finalStates, alphaSize);

    updateTransitions(&clonedLayer.transitions, alphaSize);

    return cP;
}


ODD *memorizeODD(ODD *odd) {
    // THIS WHOLE FUNCTION MUST BE UPDATED
    ODD clonedODD = *odd;
    ODD *cP = &clonedODD;

    int maxWidth = 0;
    for (int i = 0; i < odd->nLayers; ++i) {

        Layer *inputLayer = &odd->layerSequence[i];
        Layer *resultLayer = malloc(sizeof(Layer));

        memorizeLayer(inputLayer);

//        free(inputLayer); // TODO CAUSES SIGSEGV, FIGURE IT OUT
        maxWidth = (resultLayer->width > maxWidth) ? resultLayer->width : maxWidth;
        *inputLayer = *resultLayer;
    }
    clonedODD.width = maxWidth;
    return cP; // TODO FIX SHALLOW CLONING ERROR
}


int expandedSetSize(Layer *layer, bool left) {

    int stateSize = left ? layer->leftStates.nStates : layer->rightStates.nStates;
    int alphaSize = layer->map.sizeAlphabet;
    int expandedSize = stateSize * alphaSize;
    return expandedSize;
}

void updateStateContainer(StateContainer *states, int newSize) {

    free(states->set);
    states->set = malloc(newSize * sizeof(State));
    for (int i = 0; i < newSize; ++i) {
        states->set[i] = i;
    }
}

void updateInitialFinal(StateContainer *states, int alphaSize) {

    int oldSetSize = states->nStates;
    int newSetSize = states->nStates * alphaSize;
    State *newSet = malloc(newSetSize * sizeof(State));

    for (int i = 0; i < oldSetSize; ++i) {
        states->set[i] *= alphaSize;
        newSet[i * alphaSize] = states->set[i];
    }

    for (int l = 0; l < newSetSize; ++l) {
        if (l % alphaSize != 0)
            newSet[l] = newSet[l - 1] + 1;
    }

    states->nStates = newSetSize;
    free(states->set);
    states->set = newSet;
}

void updateTransitions(TransitionContainer *transitions, int alphaSize) {

    int newTransSize = transitions->nTransitions * alphaSize;
    Transition *newTrans = malloc(newTransSize * sizeof(Transition));

    for (int i = 0; i < transitions->nTransitions; ++i) {

        int s1 = transitions->set[i].s1;
        int a = transitions->set[i].a;
        int s2 = transitions->set[i].s2;

        for (int j = 0; j < alphaSize; ++j) {
            newTrans[j + (alphaSize * i)].s1 = s1 * alphaSize + j;
            newTrans[j + (alphaSize * i)].a = a;
            newTrans[j + (alphaSize * i)].s2 = s2 * alphaSize + a;
        }
    }

    transitions->nTransitions = newTransSize;
    free(transitions->set);
    transitions->set = newTrans;
}