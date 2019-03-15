// Copyright 2019 Andreas Ommundsen
// This file is licensed under MIT License, as specified in the file LICENSE located at the root folder of this repository.

#include "odd.h"
#include "cloneKill.h"
#include <stdlib.h>

/*
 * Input:
 *      *states : The StateContainer to be updated.
 *      newSize : The new size of the state set.
 * Effect:
 *      Memory of *states is freed and then a new array holding states [0 ... newSize-1] is set to that memory.
 */
void updateStateContainer(StateContainer *states, int alphaSize);

/*
 * Input:
 *      *transitions: The layer.transitions TransitionsContainer that we wish to update.
 *      alphaSize : The layer.map.sizeAlphabet.
 * Effect:
 *      *transitions should be updated to show the mappings from the expanded left states to the expanded right states.
 */
void updateTransitions(TransitionContainer *transitions, int alphaSize);

Layer *memorizeLayer(Layer *layer) {

    Layer *clonedLayer = cloneLayer(layer);

    int alphaSize = clonedLayer->map.sizeAlphabet;
    int newLeftSize = clonedLayer->leftStates.nStates * alphaSize; // expandedSetSize(layer, true);
    int newRightSize = clonedLayer->rightStates.nStates * alphaSize; // expandedSetSize(layer, false);

    clonedLayer->width = (newLeftSize > newRightSize) ? newLeftSize : newRightSize;

    updateStateContainer(&clonedLayer->leftStates, alphaSize);

    updateStateContainer(&clonedLayer->rightStates, alphaSize);

    if (clonedLayer->initialFlag)
        updateStateContainer(&clonedLayer->initialStates, alphaSize);

    if (clonedLayer->finalFlag)
        updateStateContainer(&clonedLayer->finalStates, alphaSize);

    updateTransitions(&clonedLayer->transitions, alphaSize);

    return clonedLayer;
}

ODD *memorizeODD(ODD *odd) {

    ODD *clonedODD = cloneODD(odd);

    int maxWidth = 0;
    for (int i = 0; i < odd->nLayers; ++i) {

        clonedODD->layerSequence[i] = *memorizeLayer(&odd->layerSequence[i]);

        maxWidth =
                (clonedODD->layerSequence[i].width > maxWidth)
                ? clonedODD->layerSequence[i].width : maxWidth;
    }
    clonedODD->width = maxWidth;
    return clonedODD;
}


void updateStateContainer(StateContainer *states, int alphaSize) {

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