// Copyright 2019 Amund Lindberg and Markus Ragnhildstveit
// This file is licensed under MIT License, as specified in the file LICENSE located at the root folder of this repository.

#include <stdlib.h>
#include <math.h>
#include "odd.h"


//Some helper functions
void setStateSize(StateContainer *oldstate, StateContainer *powerstate);
void bitshift(int *S, int sz);
int findStateIndex(Layer *layer, State s2, int j);
int hasCommonStates(int *S, StateContainer rightstate, StateContainer finstate);
int isSubSet(int *subS, int *S, int sz);

void powerSetODD(ODD *odd, ODD *result)
{
    result->nLayers = odd->nLayers;
    result->layerSequence = malloc(odd->nLayers * sizeof(Layer));
    result->width = 0;

    for (int i = 0; i < odd->nLayers; i++)
    {
        // Layer powerlayer;
        powerSetLayer(&odd->layerSequence[i], &result->layerSequence[i]);

        // result->layerSequence[i] = powerlayer;

        if (result->layerSequence[i].width > result->width)
        {
            result->width = result->layerSequence[i].width;
        }
    }

}

void powerSetLayer(Layer *layer, Layer *result)
{
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

    if (layer->initialStates.nStates > 0) {
        setStateSize(&layer->initialStates, &result->initialStates);
    } 
    result->initialStates.nStates = 0;
    
    if (layer->finalStates.nStates > 0) {
        setStateSize(&layer->finalStates, &result->finalStates);
    }
    result->finalStates.nStates = 0;
    
    
    int maxsz = pow(2, layer->width);

    //ALLOCATE FOR POWER-TRANSITIONS
    //TODO: What is max possible transitions? 
    result->transitions.nTransitions = maxsz;
    result->transitions.set = malloc(maxsz * sizeof(Transition));

    int *S = calloc(layer->width, sizeof(int));
    int *initialS = calloc(layer->width, sizeof(int));
    for(int i = 0; i < layer->initialStates.nStates; i++)
    {
        int posNotFound = 1; 
        for(int j = 0; posNotFound && j < layer->leftStates.nStates; j++)
        {
            if (layer->initialStates.set[i] == layer->leftStates.set[j]) {
                initialS[j] = 1;
                posNotFound = 0;
            }
        }
    }

    for (int i = 0; i < maxsz; i++)
    {

        int order = orderSet(S, layer);

        if (i < result->leftStates.nStates)
        {
            result->leftStates.set[i] = order;

            //TODO:::::::::
            for (int j = 0; j < layer->map.sizeAlphabet; j++)
            {

                int a = layer->transitions.set[j].a;
                int ordernext = orderSet(next(S, a, layer), layer);

                if (ordernext != 0)
                {
                    result->transitions.set[i].s1 = order;
                    result->transitions.set[i].a = a;
                    result->transitions.set[i].s2 = ordernext;
                }
            }
            ////////////////

            if (layer->initialStates.nStates > 0 && i > 0 && isSubSet(S, initialS, layer->width))
            {
                result->initialStates.set[result->initialStates.nStates] = order;
                result->initialStates.nStates++;
            }
        }

        if (i < result->rightStates.nStates)
        {
            result->rightStates.set[i] = order;

            if (layer->finalStates.nStates > 0 && hasCommonStates(S, layer->rightStates, layer->finalStates))
            {
                result->finalStates.set[result->finalStates.nStates] = order;
                result->finalStates.nStates++;
            }
        }
        bitshift(S, layer->width);
    }

    free(initialS);
    result->width = fmax(result->leftStates.nStates, result->rightStates.nStates);
}

int isSubSet(int *subS, int *S, int sz)
{
    for(int i = 0; i < sz; i++)
    {
        if (subS[i] == 1 && S[i] != 1) {
            return 0;
        }
    }

    return 1;
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

    for (size_t i = 0; i < layer->width; i++)
    {
        if (S[i])
        {
            for (size_t j = 0; j < layer->transitions.nTransitions; j++)
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