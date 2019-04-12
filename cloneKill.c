// Copyright 2019 Andreas Ommundsen
// This file is licensed under MIT License, as specified in the file LICENSE located at the root folder of this repository.

#include <stdlib.h>
#include "cloneKill.h"
#include "odd.h"
#include <string.h>

ODD *cloneODD(ODD *original) {

    ODD *clone = malloc(sizeof(ODD)); // FREE ME

    clone->width = original->width;
    clone->nLayers = original->nLayers;
    clone->layerSequence = malloc(clone->nLayers * sizeof(Layer)); // FREE ME
    for (int i = 0; i < clone->nLayers; ++i) {
        Layer* temp = cloneLayer(&original->layerSequence[i]);
        clone->layerSequence[i] = *temp;
        free(temp);
    }
    return clone;
}

Layer *cloneLayer(Layer *original) {

    Layer *clone = malloc(sizeof(Layer)); // FREE ME

    // Layer Info
    clone->width = original->width;
    clone->initialFlag = original->initialFlag;
    clone->finalFlag = original->finalFlag;

    // Layer AlphabetMap
    clone->map.sizeAlphabet = original->map.sizeAlphabet;

    clone->map.N2S =
            malloc( // FREE ME
                    clone->map.sizeAlphabet *
                    sizeof(char *)
            );
    for (int j = 0; j < clone->map.sizeAlphabet; ++j) {
        clone->map.N2S[j] = strdup(original->map.N2S[j]);
    }

    clone->map.S2N =
            malloc( // FREE ME
                    clone->map.sizeAlphabet *
                    sizeof(int)
            );
    for (int k = 0; k < clone->map.sizeAlphabet; ++k) {
        clone->map.S2N[k] = original->map.S2N[k];
    }

    // Layer leftStates
    clone->leftStates.nStates = original->leftStates.nStates;
    clone->leftStates.set =
            malloc( // FREE ME
                    clone->leftStates.nStates *
                    sizeof(State)
            );
    for (int l = 0; l < clone->leftStates.nStates; ++l) {
        clone->leftStates.set[l] = original->leftStates.set[l];
    }

    // Layer initialStates.
    clone->initialStates.nStates = original->initialStates.nStates;
    clone->initialStates.set =
            malloc( // FREE ME
                    clone->initialStates.nStates *
                    sizeof(State)
            );
    for (int m = 0; m < clone->initialStates.nStates; ++m) {
        clone->initialStates.set[m] = original->initialStates.set[m];
    }

    // Layer rightStates.
    clone->rightStates.nStates = original->rightStates.nStates;
    clone->rightStates.set =
            malloc( // FREE ME
                    clone->rightStates.nStates *
                    sizeof(State)
            );
    for (int n = 0; n < clone->rightStates.nStates; ++n) {
        clone->rightStates.set[n] = original->rightStates.set[n];
    }

    // Layer finalStates.
    clone->finalStates.nStates = original->finalStates.nStates;
    clone->finalStates.set =
            malloc( // FREE ME
                    clone->finalStates.nStates *
                    sizeof(State)
            );
    for (int o = 0; o < clone->finalStates.nStates; ++o) {
        clone->finalStates.set[o] = original->finalStates.set[o];
    }

    // Layer transitions
    clone->transitions.nTransitions = original->transitions.nTransitions;
    clone->transitions.set =
            malloc( // FREE ME
                    clone->transitions.nTransitions *
                    sizeof(Transition)
            );
    for (int p = 0; p < clone->transitions.nTransitions; ++p) {
        clone->transitions.set[p] = original->transitions.set[p];
    }

    return clone;
}

void killODD(ODD *target) {

    for (int i = 0; i < target->nLayers; ++i)
        killLayer(&target->layerSequence[i]);
    free(target->layerSequence);
    free(target);
}

void killLayer(Layer *target) {

    free(target->transitions.set);
    free(target->finalStates.set);
    free(target->rightStates.set);
    free(target->initialStates.set);
    free(target->leftStates.set);
    free(target->map.S2N);
    for (int i = 0; i < target->map.sizeAlphabet; ++i)
        free(target->map.N2S[i]);
    free(target->map.N2S);

}
