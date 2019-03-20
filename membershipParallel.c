// Copyright 2019 Haavard Notland & Andreas Steinvik
// This file is licensed under MIT License, as specified in the file LISENSE located at the root folder of this repository.

#include "odd.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>



State* findState(StateContainer* states, State* s){
    for(int i=0; i<states->nStates;i++){
        if (states->set[i]==*s){
            return &states->set[i];
        }
    }
    return NULL;
} // returns address of state t or NULL if t is not there


bool tryPath(ODD* odd, int* numSeq, State* state, Layer* layer, int seq){

    Transition* t;
    int result = 0;

    #pragma omp parallel for
    //TODO Replace for loop with parallelized calls to tryPath
    for(int i=0; i < layer->transitions.nTransitions; i++){

        t = &(layer->transitions.set[i]);

        if(t->s1 == *state && t->a == numSeq[seq]){
            if(layer->finalFlag){
                if(findState(&(layer->finalStates),&t->s2)==NULL){
                    continue;
                }
                else{
                    #pragma omp critical
                    result = 1;
                }

            }
            bool b = tryPath(odd, numSeq, &t->s2, &(odd->layerSequence[seq+1]), seq+1);
            if(b){
                #pragma omp critical
                result = 1;
            }
        }
    }
    return result;
}

bool numMembership(ODD* odd, int* numSeq){

    Layer* initial = &(odd -> layerSequence[0]);
    //TODO Replace for loop with parallelized calls to tryPath
    omp_set_num_threads(initial->initialStates . nStates);
    int result = 0;
    #pragma omp parallel
    {
        int myID = omp_get_thread_num();
        bool b = tryPath(odd, numSeq, &(initial->initialStates.set[myID]), initial, 0);
        if(b){
            #pragma omp critical
            result = 1;
        }
    }
    return result;
}




bool tryStringPath(ODD* odd, char** strSeq, State* state, Layer* layer, int seq){

    int currentInt;
    int partition;
    int lowerBound;
    int upperBound;
    Transition* t;

    lowerBound = 0;
    upperBound = layer->map.sizeAlphabet;
    partition = (upperBound + lowerBound) / 2;


    while(true){

        int comparison =strcmp(layer->map.N2S[layer->map.S2N[partition]],strSeq[seq]);

        if(comparison == 0){
            currentInt = layer->map.S2N[partition];
            break;
        }

            //No map found
        else if(upperBound == lowerBound){
            return 0;
        }
        else if(comparison < 0){
            lowerBound = partition;
        }
        else{
            upperBound = partition;
        }
        partition = (upperBound + lowerBound) / 2;
    }

    int result = 0;
    #pragma omp parallel for
    //TODO Replace for loop with parallelized calls to tryPath
    for(int i=0; i < layer->transitions.nTransitions; i++){

        t = &(layer->transitions.set[i]);

        if(t->s1 == *state && t->a == currentInt){
            if(layer->finalFlag){
                if(findState(&(layer->finalStates),&t->s2)==NULL){
                    continue;
                }
                else{
                    #pragma omp critical
                    result = 1;
                }
            }
            bool b = tryStringPath(odd, strSeq, &(t->s2), &(odd->layerSequence[seq+1]), seq+1);
            if(b){
                #pragma omp critical
                result = 1;
            }
        }
    }
    return result;
}

bool strMembership(ODD* odd, char** strSeq){

    Layer* initial = &(odd -> layerSequence[0]);

    //TODO Replace for loop with parallelized calls to tryPath
    omp_set_num_threads(initial->initialStates . nStates);
    int result = 0;
    #pragma omp parallel
    {
        int myID = omp_get_thread_num();
        bool b = tryStringPath(odd, strSeq, &(initial->initialStates.set[myID]), initial, 0);
        if(b){
            #pragma omp critical
            result = 1;
        }
    }
    return result;

}