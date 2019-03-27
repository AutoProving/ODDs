/**
 * Copyright 2019 Lucia Fuentes Villodres
 * This file is licensed under MIT License, as specified in the file LISENSE located at the root folder of this repository.
 * */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <omp.h>
#include "odd.h"


/**
 * Helper function that receives a layer, a leftState and a symbol, and returns
 * the first transition in the layer that goes from that state and has that symbol
 * If the transition doesn't exist, returns NULL
 * */
Transition* find_transition(Layer* here, State leftState, NumSymbol symbol){
    Transition* transitions = here->transitions.set;
    int num_transitions = here->transitions.nTransitions;

    for (int i=0; i<num_transitions; i++){
        if (transitions[i].s1 == leftState && transitions[i].a == symbol) return &transitions[i];
    }
    return NULL;
}


Layer* makeCompleteLayer(Layer *layer){
    int i, j, k;
    Layer * result = (Layer*)calloc(sizeof(Layer), 1);

    result->initialFlag = layer->initialFlag;
    result->finalFlag = layer->finalFlag;

    //rightStates
    int rLast = layer->rightStates.set[layer->rightStates.nStates-1];
    State rigth_deadState = (State)rLast+1;

    result->rightStates.set = (State*)calloc(layer->rightStates.nStates+1, sizeof(State));
    memcpy(result->rightStates.set, layer->rightStates.set, layer->rightStates.nStates*sizeof(State));
    result->rightStates.set[layer->rightStates.nStates] = rigth_deadState;
    result->rightStates.nStates = layer->rightStates.nStates+1;

    //leftStates
    int lLast = layer->leftStates.set[layer->leftStates.nStates-1];
    State left_deadState = (State)lLast+1;

    result->leftStates.set = (State*)calloc(layer->leftStates.nStates+1, sizeof(State));
    memcpy(result->leftStates.set, layer->leftStates.set, layer->leftStates.nStates*sizeof(State));
    result->leftStates.set[layer->leftStates.nStates] = left_deadState;
    result->leftStates.nStates = layer->leftStates.nStates+1;

    result->width = (result->leftStates.nStates > result->rightStates.nStates ? 
                     result->leftStates.nStates : result->rightStates.nStates);

    result->finalStates.set = (State*)calloc(layer->finalStates.nStates, sizeof(State));
    memcpy(result->finalStates.set, layer->finalStates.set, layer->finalStates.nStates*sizeof(State));
    result->finalStates.nStates = layer->finalStates.nStates;

    result->initialStates.set = (State*)calloc(layer->initialStates.nStates, sizeof(State));
    memcpy(result->initialStates.set, layer->initialStates.set, layer->initialStates.nStates*sizeof(State));
    result->initialStates.nStates = layer->initialStates.nStates;

    result->map.N2S = (char**)calloc(sizeof(char*), layer->map.sizeAlphabet);
    for(int i=0; i<layer->map.sizeAlphabet; i++){
        result->map.N2S[i] = (char*)calloc(sizeof(char), layer->map.sizeAlphabet);
        memcpy(result->map.N2S[i], layer->map.N2S[i], layer->map.sizeAlphabet*sizeof(char));
    }

    result->map.S2N = (int*)calloc(sizeof(int), layer->map.sizeAlphabet);
    memcpy(result->map.S2N, layer->map.S2N, layer->map.sizeAlphabet*sizeof(int));

    result->map.sizeAlphabet = layer->map.sizeAlphabet;

    //n of transitions = n of leftstates * size alphabet
    result->transitions.nTransitions = result->leftStates.nStates*layer->map.sizeAlphabet;
    result->transitions.set = (Transition*)calloc(sizeof(Transition), result->transitions.nTransitions);

    for(i=0, k=0; i<result->leftStates.nStates; i++){
        State current_leftstate = result->leftStates.set[i];
        for(j=0; j<result->map.sizeAlphabet; j++, k++){
            if (find_transition(layer, current_leftstate, (NumSymbol)j) != NULL)
                memcpy(&result->transitions.set[k], find_transition(layer, current_leftstate, (NumSymbol)j), sizeof(Transition));
            else{
                Transition* newTransition = (Transition*)calloc(sizeof(Transition), 1);
                newTransition->s1 = current_leftstate;
                newTransition->a = (NumSymbol)j;
                newTransition->s2 = rigth_deadState;
                memcpy(&result->transitions.set[k], newTransition, sizeof(Transition));
                free(newTransition);
            }
        }
    }

    return result;
}


ODD* makeCompleteODD(ODD* odd){
    if (!odd) return NULL;
    int i;
    ODD* result = (ODD*)calloc(sizeof(ODD), 1);

    result->layerSequence = (Layer*)calloc(sizeof(Layer), odd->nLayers);
    result->nLayers = odd->nLayers;

    #pragma omp parallel //parallel block
    {
        #pragma omp for //each layer is completed in parallel
        for (i=0; i<odd->nLayers; i++){
            Layer* res = makeCompleteLayer(&odd->layerSequence[i]);
            memcpy(&result->layerSequence[i], res, sizeof(Layer));
            free(res);
        }
    }

    //update width
    int current_width = 0;
    for(i=0; i<result->nLayers; i++){
        if(result->layerSequence[i].width > current_width){
            current_width = result->layerSequence[i].width;
            result->width = current_width;
        }
    }

    return result;
}


