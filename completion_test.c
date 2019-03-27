/**
 * Copyright 2019 Lucia Fuentes Villodres
 * This file is licensed under MIT License, as specified in the file LISENSE located at the root folder of this repository.
 * */

#include <stdlib.h>
#include "odd.h"

/**
 * Helper function that checks if a layer is complete
 * (checks each leftstate has a transition for each symbol in the alphabet)
 * */
bool isComplete(Layer* lay){
    bool answer = false;
    if (lay->leftStates.nStates*lay->map.sizeAlphabet == lay->transitions.nTransitions)
        answer = true;
    return answer;
}


int main(){
    ODD *initial = (ODD*)calloc(sizeof(ODD), 1); 
    readODD("incompleteODD.txt", initial);
    ODD *result = makeCompleteODD(initial);
    printODD("completeODD.txt", result);

    for(int i=0; i<result->nLayers; i++){
        fprintf(stdout, "              ---------LAYER %d---------\n"
                        "               ORIGINAL  |    COMPLETE\n"
                        "is complete?    %s    |     %s\n"
                        "ntransitions     %d       |       %d\n\n"
                , i+1
                , isComplete(&initial->layerSequence[i]) ? "true " : "false"
                , isComplete(&result->layerSequence[i]) ? "true " : "false"
                , initial->layerSequence[i].transitions.nTransitions
                , result->layerSequence[i].transitions.nTransitions);
    }

    char* msg = "ODD IS COMPLETE";
    for(int i=0; i<result->nLayers; i++){
        if (isComplete(&result->layerSequence[i]) == false){
            msg = "ODD IS NOT COMPLETE";
        }
    }
    fprintf(stdout, "%s ---> result in completeODD.txt\n", msg);
    free(result);
    free(initial);
    return EXIT_SUCCESS;
}


