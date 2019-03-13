#include <stdlib.h>
#include <string.h>
#include "cloneKill.h"
#include "odd.h"

ODD *cloneODD(ODD *input) {

    ODD *clone = malloc(sizeof(ODD)); // FREE ME

    clone->width = input->width;
    clone->nLayers = input->nLayers;
    clone->layerSequence = malloc(clone->nLayers * sizeof(Layer)); // FREE ME
    for (int i = 0; i < clone->nLayers; ++i) {

        // Layer Info
        clone->layerSequence[i].width = input->layerSequence[i].width;
        clone->layerSequence[i].initialFlag = input->layerSequence[i].initialFlag;
        clone->layerSequence[i].finalFlag = input->layerSequence[i].finalFlag;

        // Layer AlphabetMap
        clone->layerSequence[i].map.sizeAlphabet = input->layerSequence[i].map.sizeAlphabet;

        clone->layerSequence[i].map.N2S =
                malloc( // FREE ME
                        clone->layerSequence[i].map.sizeAlphabet *
                        sizeof(char *)
                );
        for (int j = 0; j < clone->layerSequence[i].map.sizeAlphabet; ++j) {
            clone->layerSequence[i].map.N2S[j] = input->layerSequence[i].map.N2S[j];
        }

        clone->layerSequence[i].map.S2N =
                malloc( // FREE ME
                        clone->layerSequence[i].map.sizeAlphabet *
                        sizeof(int)
                );
        for (int k = 0; k < clone->layerSequence[i].map.sizeAlphabet; ++k) {
            clone->layerSequence[i].map.N2S[k] = input->layerSequence[i].map.N2S[k];
        }

        // Layer leftStates
        clone->layerSequence[i].leftStates.nStates = input->layerSequence[i].leftStates.nStates;
        clone->layerSequence[i].leftStates.set =
                malloc( // FREE ME
                        clone->layerSequence[i].leftStates.nStates *
                        sizeof(State)
                );
        for (int l = 0; l < clone->layerSequence[i].leftStates.nStates; ++l) {
            clone->layerSequence[i].leftStates.set[l] = input->layerSequence[i].leftStates.set[l];
        }

        // Layer initialStates.
        clone->layerSequence[i].initialStates.nStates = input->layerSequence[i].initialStates.nStates;
        clone->layerSequence[i].initialStates.set =
                malloc( // FREE ME
                        clone->layerSequence[i].initialStates.nStates *
                        sizeof(State)
                );
        for (int m = 0; m < clone->layerSequence[i].initialStates.nStates; ++m) {
            clone->layerSequence[i].initialStates.set[m] = input->layerSequence[i].initialStates.set[m];
        }

        // Layer rightStates.
        clone->layerSequence[i].rightStates.nStates = input->layerSequence[i].rightStates.nStates;
        clone->layerSequence[i].rightStates.set =
                malloc( // FREE ME
                        clone->layerSequence[i].rightStates.nStates *
                        sizeof(State)
                );
        for (int n = 0; n < clone->layerSequence[i].rightStates.nStates; ++n) {
            clone->layerSequence[i].rightStates.set[n] = input->layerSequence[i].rightStates.set[n];
        }

        // Layer finalStates.
        clone->layerSequence[i].finalStates.nStates = input->layerSequence[i].finalStates.nStates;
        clone->layerSequence[i].finalStates.set =
                malloc( // FREE ME
                        clone->layerSequence[i].finalStates.nStates *
                        sizeof(State)
                );
        for (int o = 0; o < clone->layerSequence[i].finalStates.nStates; ++o) {
            clone->layerSequence[i].finalStates.set[o] = input->layerSequence[i].finalStates.set[o];
        }

        // Layer transitions
        clone->layerSequence[i].transitions.nTransitions = input->layerSequence[i].transitions.nTransitions;
        clone->layerSequence[i].transitions.set =
                malloc( // FREE ME
                        clone->layerSequence[i].transitions.nTransitions *
                        sizeof(Transition)
                );
        for (int p = 0; p < clone->layerSequence[i].transitions.nTransitions; ++p) {
            clone->layerSequence[i].transitions.set[p] = input->layerSequence[i].transitions.set[p];
        }
    }
    return clone;
}
