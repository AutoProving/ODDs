// Copyright 2019 Amund Lindberg and Markus Ragnhildstveit
// This file is licensed under MIT License, as specified in the file LICENSE located at the root folder of this repository.

#include <stdlib.h>
#include <math.h>
#include "odd.h"

long long choose(int n,int k);
long long setStateSize(StateContainer *initState, StateContainer *state);
void bitshift(int *s, int sz);

int orderSet(int* S, Layer* layer) // maps subsets of integers to numbers. S has layer.width positions. S[i]=1 indicates that i belongs to S
{
    int sum = 0;
    for(int i = 0; i < layer->width; i++)
    {
        sum += S[i] * pow(2, i);
    }
    
    return sum;
}

int* next(int* S, NumSymbol a, Layer* layer) // Returns the set obtained by reading symbol a from some state in set S.  
{
    int *sout = calloc(layer->width, sizeof(int));

    for(size_t i = 0; i < layer->width; i++)
    {
        if (S[i]) {
            for(size_t j = 0; j < layer->transitions.nTransitions; j++)
            {
                if (layer->transitions.set[j].s1 == layer->leftStates.set[i] && layer->transitions.set[j].a == a) {
                    sout[layer->transitions.set[j].s2] = 1;
                }
                
            }
            
        }
        
    }
}

void powerSetLayer(Layer* layer, Layer* result)
{   
    result->initialFlag = layer->initialFlag;
    result->finalFlag = layer->finalFlag;

    long long initsz = setStateSize(&layer->initialStates, &result->initialStates);
    long long leftsz = setStateSize(&layer->leftStates, &result->leftStates);
    long long rightsz = setStateSize(&layer->rightStates, &result->rightStates);
    long long finsz = setStateSize(&layer->finalStates, &result->finalStates);
    
    //long long maxinitsz = ((layer->leftStates.nStates) >= (layer->rightStates.nStates)) ? (layer->leftStates.nStates) : (layer->rightStates.nStates);
    //^ = layer->width

    long long maxsz = pow(2, layer->width);

    result->transitions.nTransitions = maxsz;
    result->transitions.set = malloc(maxsz * sizeof(int)); //How many transitions should it be in the new layer?
    
    int *s = calloc(layer->width, sizeof(int));

    for(long long i = 0; i < maxsz; i++)
    {
        int order = orderSet(s, layer);

        if (i < leftsz) {
            result->leftStates.set[i] = order;
        }

        if (i < rightsz) {
            result->rightStates.set[i] = order;           
        }

        if (i < initsz) {
            result->initialStates.set[i] = order;          
        }

        if (i < finsz && hasCommonStates(s, layer->rightStates, layer->finalStates)) {
            result->finalStates.set[i] = order;           
        }

        /* how to update this..m
        result->transitions[i].s1 = 
        result->transitions[i].a = 
        result->transitions[i].s2 = 
        */

        bitshift(s, layer->width);
    }
}

int hasCommonStates(int *s, StateContainer rightstate, StateContainer finstate) {
    for(size_t i = 0; i < rightstate.nStates; i++)
    {
        if (s[i]) {
            for(size_t j = 0; j < finstate.nStates; j++)
            {
                if (finstate.set[j] == rightstate.set[i]) {
                    return 1;
                }   
            }   
        }
    }

    return 0;
}

void bitshift(int *s, int sz)
{
    for(size_t i = 0; i < sz; i++)
    {
        if (s[i] != 1) {
            s[i] = 1;
            return;
        } 
        else
        {
            s[i] = 0;
        }
    }
}

//initState as in original
long long setStateSize(StateContainer *initState, StateContainer *state)
{
    long long sz = pow(2, initState->nStates);
    
    state->nStates = sz;
    state->set = malloc(sz * sizeof(int));
    
    return sz;
}

void powerSetODD(ODD* odd, ODD* result)
{
    result->width = 0;
    result->nLayers = odd->nLayers;
    result->layerSequence = malloc(odd->nLayers * sizeof(Layer));

    for (int i = 0; i < odd->width; i++) {

        Layer powerlayer;
        powerSetLayer(&odd->layerSequence[i], &powerlayer);
        result->layerSequence[i] = powerlayer;
        
        if (powerlayer.width > result->width) {
            result->width = powerlayer.width;
        }
    }

}

long long choose(int n,int k)
{
    long long ans=1;
    k=k>n-k?n-k:k;
    int j=1;
    for(;j<=k;j++,n--)
    {
        if(n%j==0)
        {
            ans*=n/j;
        }else
        if(ans%j==0)
        {
            ans=ans/j*n;
        }else
        {
            ans=(ans*n)/j;
        }
    }
    return ans;
}