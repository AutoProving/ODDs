// Copyright 2019 Andreas Ommundsen
// This file is licensed under MIT License, as specified in the file LISENSE located at the root folder of this repository.

#include "odd.h"

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
 *      alphabetSize : The layer.map.sizeAlphabet.
 * Effect:
 *      *states should be updated to match the expanded state numbers.
 */
void updateInitialFinal(StateContainer *states, int alphaSize);

void memorizeLayer(Layer *layer, Layer *result) {

    Layer clonedLayer = *layer;
    Layer *cP = &clonedLayer;

    int newLeftSize = expandedSetSize(layer, true);
    int newRightSize = expandedSetSize(layer, false);

    clonedLayer.width = (newLeftSize > newRightSize) ? newLeftSize : newRightSize;

    clonedLayer.leftStates.nStates = newLeftSize;
    updateStateContainer(&clonedLayer.leftStates, newLeftSize);

    clonedLayer.rightStates.nStates = newRightSize;
    updateStateContainer(&clonedLayer.rightStates, newRightSize);

    if (clonedLayer.initialFlag)
        updateInitialFinal(&clonedLayer.initialStates, clonedLayer.map.sizeAlphabet);

    if (clonedLayer.finalFlag)
        updateInitialFinal(&clonedLayer.finalStates, clonedLayer.map.sizeAlphabet);

    free(result);
    *result = *cP;
}


void memorizeODD(ODD *odd, ODD *result) {

    ODD clonedODD = *odd;
    ODD *cP = &clonedODD;

    for (int i = 0; i < odd->nLayers; ++i) {

        Layer *inputLayer = &odd->layerSequence[i];
        Layer *resultLayer = malloc(sizeof(Layer));

        memorizeLayer(inputLayer, resultLayer);

//        free(inputLayer); // TODO CAUSES SIGSEGV, FIGURE IT OUT
        *inputLayer = *resultLayer;
    }
    free(result);
    *result = *cP;
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
        newSet[i * alphaSize] =
                states->set[i] == 0 ?
                0 :
                states->set[i];
    }

    for (int l = 0; l < newSetSize; ++l) {
        if (l % alphaSize != 0)
            newSet[l] = newSet[l - 1] + 1;
    }

    states->nStates = newSetSize;
    free(states->set);
    states->set = newSet;
}
