// Copyright 2019 GUILLERMO HOYO BRAVO
// This file is licensed under MIT License, as specified in the file LISENSE located at the root folder of this repository.

#include <stdlib.h>
#include <stdio.h>
#include <odd.h>

bool negation_layer(Layer B, Layer noB);
void negation_odd(odd D, odd ,noD);

bool negation_layer(Layer B, Layer noB){
    
    bool conditions[2];
    
    conditions[0] = test_deterministic_layer(B);
    conditions[1] = test_complete_layer(B);
    
    if(conditions[0] == FALSE || conditions[1] == FALSE)
        return FALSE;
    
    if(B->finalFalg == 1){
        noB->map = B->map;
        noB->leftStates = B->leftStates;
        noB->rightStates = B->rightStates;
        noB->initialStates = B->initalStates;
        noB->finalStates = B->finalStates;
        noB->transitions = B->transitions;
        noB->initalFlag = B->initalFlag;
        noB->finalFlag = (B->rightStates - B->leftStates);
        
        return TRUE;
    }
    
    else{
        noB->map = B->map;
        noB->leftStates = B->leftStates;
        noB->rightStates = B->rightStates;
        noB->initialStates = B->initalStates;
        noB->finalStates = B->finalStates;
        noB->transitions = B->transitions;
        noB->initalFlag = B->initalFlag;
        noB->finalFlag = B->finalFlag;
        
        return FALSE;
    }
}

void negation_odd(odd D, odd noD){
    int i = 0;
    bool lay[D->nLayers];
    
    for(i = 0; i < D->nLayers; i++){
        lay[i] = negation_layer(D->layerSequence[i], noD->layerSequence[i]);
    }
    
    return;
}
