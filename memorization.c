// Copyright 2019 Andreas Ommundsen
// This file is licensed under MIT License, as specified in the file LICENSE located at the root folder of this repository.

#include <stdlib.h>
#include "cloneKill.h"
#include "odd.h"

#ifdef _OPENMP
#   include <omp.h>
#endif

/**
 * @param states The StateContainer that we wish to update.
 * @param alphaSize The layer.map.sizeAlphabet belonging to the same layer as the input *states.
 * @Effect *states are updated to show their post-memorization expansion values with regard to the size of the alphabet.
 */
void updateStateContainer(StateContainer *states, int alphaSize);

/**
 * @param transitions The layer.transitions TransitionsContainer that we wish to update.
 * @param alphaSize The layer.map.sizeAlphabet belonging to the same layer as the input *transitions.
 * @Effect *transitions are updated to show the mappings from the expanded left states to the expanded right states.
 */
void updateTransitions(TransitionContainer *transitions, int alphaSize);

ODD *memorizeODD(ODD *odd) {

    ODD *clonedODD = cloneODD(odd);

    int maxWidth = 0;

# pragma omp parallel // num_threads(threadCount)
#   pragma omp for
    for (int i = 0; i < clonedODD->nLayers; ++i) {
        Layer *temp = memorizeLayer(&clonedODD->layerSequence[i]);
        killLayer(&clonedODD->layerSequence[i]);
        clonedODD->layerSequence[i] = *temp;
        free(temp);

#       pragma omp critical
        maxWidth =
                (clonedODD->layerSequence[i].width > maxWidth)
                ? clonedODD->layerSequence[i].width : maxWidth;
    }
    clonedODD->width = maxWidth;
    return clonedODD;
}

Layer *memorizeLayer(Layer *layer) {

    Layer *clonedLayer = cloneLayer(layer);

    int alphaSize = clonedLayer->map.sizeAlphabet;
    int newLeftSize = clonedLayer->leftStates.nStates * alphaSize;
    int newRightSize = clonedLayer->rightStates.nStates * alphaSize;

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