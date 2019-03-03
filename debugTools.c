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
        for (int i = 0; i < odd->nLayers; ++i) {
            printf("Layer[%d]:\n", i);
            showLayer(&odd->layerSequence[i]);
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
        printf("--Initial states set: ");
        for (int i = 0; i < layer->initialStates.nStates; ++i) {
            printf("%d ", layer->initialStates.set[i]);
        }
        printf("\n");
    }

    printf("-Final flag: %d\n", layer->finalFlag);
    if (layer->finalFlag) {
        printf("--Final states set: ");
        for (int i = 0; i < layer->finalStates.nStates; ++i) {
            printf("%d ", layer->finalStates.set[i]);
        }
        printf("\n");
    }

    printf("-States:\n");
    int maxStates =
            (layer->leftStates.nStates > layer->rightStates.nStates) ?
            layer->leftStates.nStates :
            layer->rightStates.nStates;

    for (int i = 0; i < maxStates; ++i) {

        if (i < layer->leftStates.nStates)
            printf("%d", layer->leftStates.set[i]);
        else
            printf(" ");

        if (i < layer->rightStates.nStates)
            printf("%5d\n", layer->rightStates.set[i]);
        else
            printf("\n");
    }

    printf("-Transitions:\n");
    for (int l = 0; l < layer->transitions.nTransitions; ++l) {
        printf("%d --{%s=%d}--> %d\n",
               layer->transitions.set[l].s1,
               layer->map.N2S[layer->transitions.set[l].a],
               layer->transitions.set[l].a,
               layer->transitions.set[l].s2
        );
    }
}
