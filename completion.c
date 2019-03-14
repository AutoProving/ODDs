//Copyright 2019 Lucia Fuentes Villodres
//This file is licensed under MIT License, as specified in the file LISENSE located at the root folder of this repository.

#include <stdlib.h>
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


void makeCompleteLayer(Layer *layer, Layer *result){
    int i, j, k;
    result->finalStates = layer->finalStates;
    result->leftStates = layer->initialStates;
    result->finalFlag =  layer->finalFlag;
    result->initialFlag = layer->initialFlag;
    result->map = layer->map;
    result->width = layer->width;

    //calls to realloc allocate memory for one more state in the layer
    State right_deadState = (State)layer->rightStates.nStates;
    result->rightStates.set = (State*)realloc(layer->rightStates.set, layer->rightStates.nStates+1);
    result->rightStates.set[layer->rightStates.nStates] = right_deadState;
    result->rightStates.nStates = layer->rightStates.nStates+1;

    State left_deadState = (State)layer->leftStates.nStates;
    result->leftStates.set = (State*)realloc(layer->leftStates.set, layer->leftStates.nStates+1);
    result->leftStates.set[result->leftStates.nStates] = left_deadState;
    result->leftStates.nStates = layer->leftStates.nStates+1;

    //n of transitions = n of states * size alphabet
    result->transitions.nTransitions = result->leftStates.nStates*layer->map.sizeAlphabet;
    result->transitions.set = (Transition*)calloc(sizeof(Transition), result->transitions.nTransitions);

    for(i=0, k=0;i<result->leftStates.nStates;i++){
        for(j=0; j<layer->map.sizeAlphabet;j++, k++){
            Transition *newTransition = find_transition(layer, result->leftStates.set[i], (NumSymbol)j); 
            if (newTransition == NULL){
                newTransition = (Transition*)calloc(1, sizeof(Transition)); 
                newTransition->s1 = result->leftStates.set[i];
                newTransition->a = (NumSymbol)j;
                newTransition->s2 = right_deadState;
            }
            result->transitions.set[k] = *newTransition;
        }
    }
}


void makeCompleteODD(ODD* odd, ODD* result){
    if (!odd) return;
    int i;
    result->layerSequence = (Layer*)calloc(sizeof(Layer), odd->nLayers);
    result->nLayers = odd->nLayers;
    result->width = odd->width;

    for (i=0; i<odd->nLayers; i++){
        makeCompleteLayer(&odd->layerSequence[i], &result->layerSequence[i]);
    }
}


