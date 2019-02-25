// Copyright 2019 YOUR NAME
// This file is licensed under MIT License, as specified in the file LISENSE located at the root folder of this repository.

#include "odd.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

State* findState(StateContainer* states, State* s){
    for(int i=0; i<states->nStates;i++){
        if (states->set[i]==*s){
            return &states->set[i];
        }
    }
    return NULL;
} // returns address of state t or NULL if t is not there 


bool tryPath(ODD* odd, int* numSeq, State* state, Layer* layer, int seq){

    Transition t;

    for(int i=0; i < layer->transitions.nTransitions; i++){

        t = layer->transitions.set[i];

        if(t.s1 == *state && t.a == numSeq[seq]){
            if(layer->finalFlag){
                if(findState(&(layer->finalStates),&t.s2)==NULL){
                    continue;
                }
                else{
                   return 1; 
                }
                
            }
            bool b = tryPath(odd, numSeq, &t.s2, &(odd->layerSequence[seq+1]), seq+1);
            if(b){
                return 1;
            }
        }
    }
    return 0;
}

bool numMembership(ODD* odd, int* numSeq){

    //Try all initial states
    for(int i=0; i < (odd -> layerSequence[0] .initialStates .nStates); i++){
        bool b = tryPath(odd, numSeq, &(odd->layerSequence[0] .initialStates . set[i]), &(odd->layerSequence[0]), 0);
        if(b){
            return 1;
        }
    }
    return 0;
}




bool tryStringPath(ODD* odd, char** strSeq, State* state, Layer* layer, int seq){

    int currentInt;
    int partition;
    int lowerBound;
    int upperBound;
    Transition t;

    lowerBound = 0;
    upperBound = layer->map.sizeAlphabet;
    partition = (upperBound + lowerBound) / 2;
    while(true){
        if(layer->map.N2S[layer->map.S2N[partition]] == strSeq[seq]){
            currentInt = layer->map.S2N[partition];
            break;
        }

        //No map forund
        else if(upperBound == lowerBound){
            return 0;
        }
        else if(layer->map.N2S[layer->map.S2N[partition]] < strSeq[seq]){
            lowerBound = partition;
        }
        else{
            upperBound = partition;
        }
        partition = (upperBound + lowerBound) / 2;
    }



    for(int i=0; i < layer->transitions.nTransitions; i++){

        t = layer->transitions.set[i];

        if(t.s1 == *state && t.a == currentInt){
            if(layer->finalFlag){
                if(findState(&(layer->finalStates),&t.s2)==NULL){
                    return 0;
                }
                else{
                   return 1; 
                }
            }
            bool b = tryStringPath(odd, strSeq, &t.s2, &(odd->layerSequence[seq+1]), seq+1);
            if(b){
                return 1;
            }
        }
    }
    return 0;
}

bool strMembership(ODD* odd, char** strSeq){

    for(int i = 0; i < odd -> layerSequence[0] . initialStates . nStates; i++){
        bool b = tryStringPath(odd, strSeq, &(odd->layerSequence[0].initialStates.set[i]), &(odd->layerSequence[0]), 0);
        if(b){
            return 1;
        }
    }
    return 0;
}

