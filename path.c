// Copyright 2019 Ole Magnus Lie
// This file is licensed under MIT License, as specified in the file LISENSE located at the root folder of this repository.

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "odd.h"

NumSymbol *getPath(ODD *odd)
{
    NumSymbol *path = malloc(odd->nLayers * sizeof(NumSymbol));
    int numLayers = odd->nLayers - 1;
    Layer finalLayer = odd->layerSequence[numLayers];
    int lastState;

    for (int finalstate = 0; finalstate < finalLayer.finalStates.nStates; finalstate++)
    {
        lastState = finalLayer.finalStates.set[finalstate];

        for (int i = numLayers; i >= 0; i--)
        {
            Layer previousLayer = odd->layerSequence[i];
            int numTransitions = previousLayer.transitions.nTransitions;
            for (int j = 0; j < numTransitions; j++)
            {
                if (previousLayer.transitions.set[j].s2 == lastState)
                {
                    path[i] = previousLayer.transitions.set[j].a;
                    lastState = previousLayer.transitions.set[j].s1;
                    break;
                }
            }
        }
    }

    return path;
}

bool isTherePath(ODD *odd)
{
    int numLayers = odd->nLayers - 1;
    Layer finalLayer = odd->layerSequence[numLayers];
    int lastState, numPathSymbols;

    for (int finalstate = 0; finalstate < finalLayer.finalStates.nStates; finalstate++)
    {
        lastState = finalLayer.finalStates.set[finalstate];
        numPathSymbols = 0;

        for (int i = numLayers; i >= 0; i--)
        {
            Layer previousLayer = odd->layerSequence[i];
            int numTransitions = previousLayer.transitions.nTransitions;
            for (int j = 0; j < numTransitions; j++)
            {
                if (previousLayer.transitions.set[j].s2 == lastState)
                {
                    lastState = previousLayer.transitions.set[j].s1;
                    numPathSymbols += 1;
                    break;
                }
            }
        }
    }

    return numPathSymbols == odd->nLayers;
}
