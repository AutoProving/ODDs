// Copyright 2019 Amund Lindberg and Markus Ragnhildstveit
// This file is licensed under MIT License, as specified in the file LICENSE located at the root folder of this repository.

#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include "odd.h"

// Some helper functions
void setStateSize(StateContainer *oldstate, StateContainer *powerstate);
void bitshift(int *S, int sz);
int findStateIndex(Layer *layer, State s2, int j);
int hasCommonStates(int *S, StateContainer rightstate, StateContainer finstate);
int isSubSet(int *subS, int *S, int sz);
void showTransitions(TransitionContainer transitions);
void showStates(StateContainer states);
void testPowerSetODD(ODD odd, ODD powerODD);
void intToBitArray(int num, int *S, int sz);
// End helper functions

// int main() {
//     int S[4];
//     intToBitArray(16, S, 4);
//     printf("%d%d%d%d\n", S[0], S[1], S[2], S[3]);
// }

ODD* powerSetODD(ODD *odd)
{
    ODD* result = malloc(sizeof(ODD));
    result->nLayers = odd->nLayers;
    result->layerSequence = malloc(odd->nLayers * sizeof(Layer));
    result->width = 0;
#pragma omp parallel for
    for (int i = 0; i < odd->nLayers; i++)
    {
        result->layerSequence[i] = *powerSetLayer(&odd->layerSequence[i]);
    }

    // Find max width of the layers
    for(int i = 0; i < odd->nLayers; i++)
    {
        if (result->layerSequence[i].width > result->width)
        {
            result->width = result->layerSequence[i].width;
        }
    }
    
    return result;
}

Layer* powerSetLayer(Layer *layer)
{
    Layer* result = malloc(sizeof(Layer));
    //SET FLAGS
    result->initialFlag = layer->initialFlag;
    result->finalFlag = layer->finalFlag;

    //SET ALPHABET MAP
    result->map.sizeAlphabet = layer->map.sizeAlphabet;
    result->map.N2S = layer->map.N2S;
    result->map.S2N = layer->map.S2N;

    //ALLOCATE FOR POWER-STATES
    setStateSize(&layer->leftStates, &result->leftStates);
    setStateSize(&layer->rightStates, &result->rightStates);

    if (layer->initialStates.nStates > 0)
    {
        setStateSize(&layer->initialStates, &result->initialStates);
    }
    result->initialStates.nStates = 0;

    if (layer->finalStates.nStates > 0)
    {
        setStateSize(&layer->finalStates, &result->finalStates);
    }
    result->finalStates.nStates = 0;

    int maxsz = pow(2, layer->width);

    //ALLOCATE FOR POWER-TRANSITIONS
    result->transitions.nTransitions = 0;
    result->transitions.set = malloc(maxsz * layer->map.sizeAlphabet * sizeof(Transition));

    int *initialS = calloc(layer->width, sizeof(int));
    for (int i = 0; i < layer->initialStates.nStates; i++)
    {
        int posNotFound = 1;
        for (int j = 0; posNotFound && j < layer->leftStates.nStates; j++)
        {
            if (layer->initialStates.set[i] == layer->leftStates.set[j])
            {
                initialS[j] = 1;
                posNotFound = 0;
            }
        }
    }

    int *S = calloc(layer->width, sizeof(int));
    for (int i = 0; i < maxsz; i++)
    {
        int order = orderSet(S, layer);
        
        if (i < result->leftStates.nStates)
        {
            // Left states:
            result->leftStates.set[i] = order;

            // Initial states:
            if (layer->initialStates.nStates > 0 && i > 0 && isSubSet(S, initialS, layer->width))
            {
                result->initialStates.set[result->initialStates.nStates] = order;
                result->initialStates.nStates++;
            }

            // Transitions:
            for (int j = 0; j < layer->map.sizeAlphabet; j++)
            {

                int a = layer->map.S2N[j];
                int ordernext = orderSet(next(S, a, layer), layer);

                if (ordernext != 0)
                {
                    result->transitions.set[result->transitions.nTransitions].s1 = order;
                    result->transitions.set[result->transitions.nTransitions].a = a;
                    result->transitions.set[result->transitions.nTransitions].s2 = ordernext;
                    result->transitions.nTransitions++;
                }
            }
        }

        if (i < result->rightStates.nStates)
        {
            // Right states:
            result->rightStates.set[i] = order;

            // Final states:
            if (layer->finalStates.nStates > 0 && hasCommonStates(S, layer->rightStates, layer->finalStates))
            {
                result->finalStates.set[result->finalStates.nStates] = order;
                result->finalStates.nStates++;
            }
        }
        bitshift(S, layer->width);
    }
    free(S);
    free(initialS);
    result->width = fmax(result->leftStates.nStates, result->rightStates.nStates);

    return result;
}

// maps subsets of integers to numbers. S has layer.width positions. S[i]=1 indicates that i belongs to S
int orderSet(int *S, Layer *layer)
{
    int sum = 0;
    for (int i = 0; i < layer->width; i++)
    {
        sum += S[i] * pow(2, i);
    }

    return sum;
}

// Returns the set obtained by reading symbol a from some state in set S.
int *next(int *S, NumSymbol a, Layer *layer)
{
    int *sout = calloc(layer->width, sizeof(int));

    for (int i = 0; i < layer->width; i++)
    {
        if (S[i])
        {
            for (int j = 0; j < layer->transitions.nTransitions; j++)
            {
                if (layer->transitions.set[j].s1 == layer->leftStates.set[i] && layer->transitions.set[j].a == a)
                {
                    sout[findStateIndex(layer, layer->transitions.set[j].s2, j)] = 1;
                }
            }
        }
    }
    return sout;
}

///////////////////////////////////////
//helper functions below

void intToBitArray(int num, int *S, int sz) {
    unsigned int mask = 1U << (sz-1);

    for (int i = 0; i < sz; i++) {
        S[i] = (num & mask) ? 1 : 0;
        num <<= 1;
    }
}

void setStateSize(StateContainer *oldstate, StateContainer *powerstate)
{
    int sz = pow(2, oldstate->nStates);
    powerstate->nStates = sz;
    // powerstate->set = calloc(sz, sizeof(State));
    powerstate->set = malloc(sz * sizeof(int));
}

void bitshift(int *S, int sz)
{
    for (size_t i = 0; i < sz; i++)
    {
        if (S[i] != 1)
        {
            S[i] = 1;
            return;
        }
        else
        {
            S[i] = 0;
        }
    }
}

int isSubSet(int *subS, int *S, int sz)
{
    for (int i = 0; i < sz; i++)
    {
        if (subS[i] == 1 && S[i] != 1)
        {
            return 0;
        }
    }

    return 1;
}

int hasCommonStates(int *S, StateContainer rightstate, StateContainer finstate)
{
    for (int i = 0; i < rightstate.nStates; i++)
    {
        if (S[i])
        {
            for (size_t j = 0; j < finstate.nStates; j++)
            {
                if (finstate.set[j] == rightstate.set[i])
                {
                    return 1;
                }
            }
        }
    }

    return 0;
}

//
int findStateIndex(Layer *layer, State s2, int j)
{
    if (layer->rightStates.set[s2] == s2)
    {
        return s2;
    }
    else
    {
        for (size_t i = 0; i < layer->rightStates.nStates; i++)
        {
            if (layer->rightStates.set[i] == s2)
            {
                return i;
            }
        }
    }

    perror("Could not find the state in next-function.");
    exit(-1);
}

void showTransitions(TransitionContainer transitions)
{
    printf("{");
    for (int i = 0; i < transitions.nTransitions; i++)
    {
        printf("(%d, %d, %d)", transitions.set[i].s1, transitions.set[i].s2, transitions.set[i].a);
        if (i != transitions.nTransitions - 1)
        {
            printf(", ");
        }
    }
    printf("}\n");
}

void showStates(StateContainer states)
{
    printf("{");
    for (int i = 0; i < states.nStates - 1; i++)
    {
        printf("%d, ", states.set[i]);
    }
    printf("%d}\n", states.set[states.nStates - 1]);
}

void testPowerSetODD(ODD odd, ODD powerODD)
{
    printf("\n------------------------ Result of PowerSetODD ---------------------------\n");
    printf("--------------------------------------------------------------------------\n");
    printf("Original ODD has width: %d and powerODD has width: %d\n", odd.width, powerODD.width);
    printf("Original ODD has %d layers and powerODD has %d layers\n", odd.nLayers, powerODD.nLayers);

    for (int i = 0; i < powerODD.nLayers; i++)
    {

        Layer pl = powerODD.layerSequence[i];
        printf("\nPowerLayer %d with initFlag = %d, finalFlag = %d\n", i, pl.initialFlag, pl.finalFlag);

        if (pl.initialStates.nStates > 0)
        {
            printf("InitialStates: ");
            showStates(pl.initialStates);
        }

        printf("LeftStates: ");
        showStates(pl.leftStates);

        printf("RightStates: ");
        showStates(pl.rightStates);

        if (pl.finalStates.nStates > 0)
        {
            printf("FinalStates: ");
            showStates(pl.finalStates);
        }

        printf("Transitions: ");
        showTransitions(pl.transitions);

        //alphabetmap
    }

    printf("--------------------------------------------------------------------------\n");
    printf("--------------------------------------------------------------------------\n");
    printf("Done printing powerODD\n");
}
