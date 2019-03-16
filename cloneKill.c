// Copyright 2019 Andreas Ommundsen
// This file is licensed under MIT License, as specified in the file LICENSE located at the root folder of this repository.

#include <stdlib.h>
#include "cloneKill.h"
#include "odd.h"

ODD *cloneODD(ODD *input) {

    ODD *clone = malloc(sizeof(ODD)); // FREE ME

    clone->width = input->width;
    clone->nLayers = input->nLayers;
    clone->layerSequence = malloc(clone->nLayers * sizeof(Layer)); // FREE ME
    for (int i = 0; i < clone->nLayers; ++i) {
        clone->layerSequence[i] = *cloneLayer(&input->layerSequence[i]);
    }
    return clone;
}

Layer *cloneLayer(Layer *input) {

    Layer *clone = malloc(sizeof(Layer)); // FREE ME

    // Layer Info
    clone->width = input->width;
    clone->initialFlag = input->initialFlag;
    clone->finalFlag = input->finalFlag;

    // Layer AlphabetMap
    clone->map.sizeAlphabet = input->map.sizeAlphabet;

    clone->map.N2S =
            malloc( // FREE ME
                    clone->map.sizeAlphabet *
                    sizeof(char *)
            );
    for (int j = 0; j < clone->map.sizeAlphabet; ++j) {
        clone->map.N2S[j] = input->map.N2S[j];
    }

    clone->map.S2N =
            malloc( // FREE ME
                    clone->map.sizeAlphabet *
                    sizeof(int)
            );
    for (int k = 0; k < clone->map.sizeAlphabet; ++k) {
        clone->map.N2S[k] = input->map.N2S[k];
    }

    // Layer leftStates
    clone->leftStates.nStates = input->leftStates.nStates;
    clone->leftStates.set =
            malloc( // FREE ME
                    clone->leftStates.nStates *
                    sizeof(State)
            );
    for (int l = 0; l < clone->leftStates.nStates; ++l) {
        clone->leftStates.set[l] = input->leftStates.set[l];
    }

    // Layer initialStates.
    clone->initialStates.nStates = input->initialStates.nStates;
    clone->initialStates.set =
            malloc( // FREE ME
                    clone->initialStates.nStates *
                    sizeof(State)
            );
    for (int m = 0; m < clone->initialStates.nStates; ++m) {
        clone->initialStates.set[m] = input->initialStates.set[m];
    }

    // Layer rightStates.
    clone->rightStates.nStates = input->rightStates.nStates;
    clone->rightStates.set =
            malloc( // FREE ME
                    clone->rightStates.nStates *
                    sizeof(State)
            );
    for (int n = 0; n < clone->rightStates.nStates; ++n) {
        clone->rightStates.set[n] = input->rightStates.set[n];
    }

    // Layer finalStates.
    clone->finalStates.nStates = input->finalStates.nStates;
    clone->finalStates.set =
            malloc( // FREE ME
                    clone->finalStates.nStates *
                    sizeof(State)
            );
    for (int o = 0; o < clone->finalStates.nStates; ++o) {
        clone->finalStates.set[o] = input->finalStates.set[o];
    }

    // Layer transitions
    clone->transitions.nTransitions = input->transitions.nTransitions;
    clone->transitions.set =
            malloc( // FREE ME
                    clone->transitions.nTransitions *
                    sizeof(Transition)
            );
    for (int p = 0; p < clone->transitions.nTransitions; ++p) {
        clone->transitions.set[p] = input->transitions.set[p];
    }

    return clone;
}

void killODD(ODD *target) {
    fprintf(stderr, "%s() IS NOT IMPLEMENTED YET!\n", __func__);
    exit(EXIT_FAILURE);
}

void killLayer(Layer *target) {
    fprintf(stderr, "%s() IS NOT IMPLEMENTED YET!\n", __func__);
    exit(EXIT_FAILURE);
}
