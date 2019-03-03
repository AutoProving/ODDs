// Copyright 2019 Andreas Ommundsen
// This file is licensed under MIT License, as specified in the file LISENSE located at the root folder of this repository.

#include "odd.h"

/*
 * Input:
 *      *layer : The layer whose number of states we are interested in.
 *      left : True for the left side StateContainer of the layer, false for the right side StateContainer.
 * Returns:
 *      i.side * |S| : The the new size of the given stateContainer
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


    free(result);
    *result = *cP;
}


void memorizeODD(ODD *odd, ODD *result) {
    printf("You've reached %s() in %s through %s, but there's nothing here yet!", __func__, __FILE__, __BASE_FILE__);
}


int expandedSetSize(Layer *layer, bool left){

    int stateSize = left ? layer->leftStates.nStates : layer->rightStates.nStates;
    int alphaSize = layer->map.sizeAlphabet;
    int expandedSize = stateSize * alphaSize;
    return expandedSize;
}

void updateStateContainer(StateContainer *states, int newSize){

    free(states->set);
    states->set = malloc(newSize * sizeof(State));
    for (int i = 0; i < newSize; ++i) {
        states->set[i] = i;
    }
}
