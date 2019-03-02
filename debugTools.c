// Copyright 2019 Andreas Ommundsen
// This file is licensed under MIT License, as specified in the file LISENSE located at the root folder of this repository.

#include "debugTools.h"

void showODD(ODD *odd) {

    printf("ODD_width: %d\n", odd->width);
    printf("ODD_nLayers: %d\n", odd->nLayers);
    if (!odd->layerSequence)
        printf("NULL pointer @ odd.layerSequence\n");
    else {
        printf("\n");
        for (int j = 0; j < odd->nLayers; ++j) {

            printf("Layer[%d]:\n", j);
            printf("-Width:%d\n", odd->layerSequence[j].width);

            printf("-Alphabet: ");
            for (int i = 0; i < odd->layerSequence[j].map.sizeAlphabet; ++i) {
                printf("%s ", odd->layerSequence[j].map.N2S[i]);
            }
            printf("\n");

            printf("-Initial flag: %d\n", odd->layerSequence[j].initialFlag);
            if (odd->layerSequence[j].initialFlag) {
                printf("--Initial state set: ");
                for (int i = 0; i < odd->layerSequence[j].initialStates.nStates; ++i) {
                    printf("%d ", odd->layerSequence[j].initialStates.set[i]);
                }
                printf("\n");
            }
            printf("-Final flag: %d\n", odd->layerSequence[j].finalFlag);
            if (odd->layerSequence[j].finalFlag) {
                printf("--Final state set: ");
                for (int i = 0; i < odd->layerSequence[j].finalStates.nStates; ++i) {
                    printf("%d ", odd->layerSequence[j].finalStates.set[i]);
                }
                printf("\n");
            }

            printf("-Left states: ");
            for (int k = 0; k < odd->layerSequence[j].leftStates.nStates; ++k) {
                printf("%d ", odd->layerSequence[j].leftStates.set[k]);
            }
            printf("\n");

            printf("-Right states: ");
            for (int k = 0; k < odd->layerSequence[j].rightStates.nStates; ++k) {
                printf("%d ", odd->layerSequence[j].rightStates.set[k]);
            }
            printf("\n");

            printf("Transition mappings:\n");
            for (int l = 0; l < odd->layerSequence[j].transitions.nTransitions; ++l) {
                printf("%d --{%s=%d}--> %d\n",
                       odd->layerSequence[j].transitions.set[l].s1,
                       odd->layerSequence[j].map.N2S[odd->layerSequence[j].transitions.set[l].a],
                       odd->layerSequence[j].transitions.set[l].a,
                       odd->layerSequence[j].transitions.set[l].s2
                );
            }
            printf("\n\n");
        }
    }
}

void showLayer(Layer *layer) {

    printf("-Width:%d\n", layer->width);

    printf("-Alphabet: ");
    for (int i = 0; i < layer->map.sizeAlphabet; ++i) {
        printf("%s ", layer->map.N2S[i]);
    }
    printf("\n");

    printf("-Initial flag: %d\n", layer->initialFlag);
    if (layer->initialFlag) {
        printf("--Initial state set: ");
        for (int i = 0; i < layer->initialStates.nStates; ++i) {
            printf("%d ", layer->initialStates.set[i]);
        }
        printf("\n");
    }
    printf("-Final flag: %d\n", layer->finalFlag);
    if (layer->finalFlag) {
        printf("--Final state set: ");
        for (int i = 0; i < layer->finalStates.nStates; ++i) {
            printf("%d ", layer->finalStates.set[i]);
        }
        printf("\n");
    }

    printf("-Left states: ");
    for (int k = 0; k < layer->leftStates.nStates; ++k) {
        printf("%d ", layer->leftStates.set[k]);
    }
    printf("\n");

    printf("-Right states: ");
    for (int k = 0; k < layer->rightStates.nStates; ++k) {
        printf("%d ", layer->rightStates.set[k]);
    }
    printf("\n");

    printf("Transition mappings:\n");
    for (int l = 0; l < layer->transitions.nTransitions; ++l) {
        printf("%d --{%s=%d}--> %d\n",
               layer->transitions.set[l].s1,
               layer->map.N2S[layer->transitions.set[l].a],
               layer->transitions.set[l].a,
               layer->transitions.set[l].s2
        );
    }
}
