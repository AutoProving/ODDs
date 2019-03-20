// Copyright 2019 Christian Stafset, Daniel Lansdaal, Konstantin Rygol
// This file is licensed under MIT License, as specified in the file LISENSE located at the root folder of this repository.
#include <stdio.h>
#include <stdlib.h>
#include "odd.h"
#include <omp.h> //also add -fopenmp to compile path

Layer deleteDeadStatesLeft(Layer* layer, int* livingCount, int* livingStates, Layer l) {
    Transition* transitions = layer->transitions.set;
    int numTransitions = layer->transitions.nTransitions;
    int numStates = layer->leftStates.nStates;
    State * states = layer->leftStates.set;
    *livingCount = 0;
    //can make faster by running through only transitions
    //go through all transitions and add states on the right into new array make sure you dont double up
    //if you can ensure transitions come from a valid left state (which should be the case as we deleted dead states from the left)

    //good idea but how do we avoid getting double ups?

    #pragma omp parallel for
    for (int i = 0; i < numStates; i++) {
        for (int j = 0; j < numTransitions; j++) {
            if (transitions[j].s2 == states[i]) {
                livingStates[*livingCount] = states[i];
                *livingCount++;
                break;
            }
        }
    }

    l.leftStates.set = livingStates;
    l.leftStates.nStates = livingCount;
    return l;
    
}

Layer deleteDeadTransitionsLeft(Layer* layer, int livingCount, int* livingStates, int* livingTransCount, Transition* livingTransitions, Layer l) {
    Transition* transitions = layer->transitions.set;
    int transCount = layer->transitions.nTransitions;
    *livingTransCount = 0;
    //can do in n*logn time because sorted
    //binary search??

    #pragma omp parallel for
    for (int i = 0; i < livingCount; i++) {
        for (int j = 0; j < transCount; j++) {
            if (transitions[j].s1 == livingStates[i]) {
                livingTransitions[*livingTransCount] = transitions[j];
                *livingTransCount++;
            }
        }
    }

    l.leftStates.set = livingTransitions;
    l.transitions.nTransitions = livingTransCount;
}

Layer deleteDeadStatesRight(Layer* layer, int* livingCount, int* livingStates, Layer l) {
    Transition* transitions = layer->transitions.set;
    int numTransitions = layer->transitions.nTransitions;
    int numStates = layer->rightStates.nStates;
    State * states = layer->rightStates.set;
    *livingCount = 0;
    //can make faster by running through only transitions
    //go through all transitions and add states on the right into new array make sure you dont double up
    //if you can ensure transitions come from a valid left state (which should be the case as we deleted dead states from the left)

    //good idea but how do we avoid getting double ups?

    
    #pragma omp parallel for
    for (int i = 0; i < numStates; i++) {
        for (int j = 0; j < numTransitions; j++) {
            if (transitions[j].s2 == states[i]) {
                livingStates[*livingCount] = states[i];
                *livingCount++;
                break;
            }
        }
    }

    l.rightStates.set = livingStates;
    l.rightStates.nStates = livingCount;
    return l;
    
}

Layer deleteDeadTransitionsRight(Layer* layer, int livingCount, int* livingStates, int* livingTransCount, Transition* livingTransitions, Layer l) {
    Transition* transitions = layer->transitions.set;
    int transCount = layer->transitions.nTransitions;
    *livingTransCount = 0;
    //can do in n*logn time because sorted
    //binary search??
    
    #pragma omp parallel for
    for (int i = 0; i < livingCount; i++) {
        for (int j = 0; j < transCount; j++) {
            if (transitions[j].s1 == livingStates[i]) {
                livingTransitions[*livingTransCount] = transitions[j];
                *livingTransCount++;
            }
        }
    }

    l.transitions.set = livingTransitions;
    l.transitions.nTransitions = livingTransCount;
}


Layer minimizeLayer(Layer * layer,Layer l,int i){ //why do we need int i ?  
    #pragma omp parallel for
    for(int j = 0; j < layer.nstates;j++){  //assume states are sorted   
        State* state_j = layer.leftStates[j];
        if (state_j != -1){
            for(int k = j; k < layer.nstates,j++){ //not sure about that ??
                State* state_k = layer.leftStates[k];
                //check whether j and k are of same type if true they have the same type  
                if checkType(&state_j,&state_k){
                    collapseLeft(&layer,&state_j,&state_k)   //where do we put right collapse ?  
                }
            } 
        }
    }
    return l
}

//takes tranitions not states as we also need NumSymbol
bool checkTypeLeft(Transition * transition_j,Transition * transition_k){
    if(transition_j.s1 == transition_k.s1)
        if(transition_j.a == transition_k.a)
            return 1;
    return 0;
}
bool checkTypeRight(Transition * transition_j,Transition * transition_k){
    if(transition_j.s2 == transition_k.s2)
        if(transition_j.a == transition_k.a)
            return 1;
    return 0;
}

void collapseLeft(Layer * layer, State * state_i, State * state_j){
    if(state_i < state_j){ //to ensure that state_i < state_j 
        State tmp =  state_i;
        state_i = state_j;
        state_j = tmp;
        }

    layer.leftStates[state_j] = -1;
    //get num symbol of state to be replaced -> go to left state, 
    for (int i = 0; i < n; i++){ //to what does this go ? This is all transitions from 
        //state x to state j. We assume the states are sorted

        Transition.s1[state_j] = Transition.s1[state_i];
    }    
}

//void collapse right
void collapseRight(Layer * layer, State * state_i, State * state_j){
    if(state_i < state_j){ //to ensure that state_i < state_j 
        State tmp =  state_i;
        state_i = state_j;
        state_j = tmp;
        }

    layer.rightStates[state_j] = -1;
    //get num symbol of state to be replaced -> go to left state, 
    for (int i = 0; i < n; i++){ //to what does this go ? This is all transitions from 
        //state x to state j. We assume the states are sorted

        Transition.s2[state_j] = Transition.s2[state_i];
    }    
}







//need to go twice, forwards and backwards to remove states which have nothing from the left and nothing from the right

ODD* minimize (ODD* odd) {

    // //create copy of ODD, I guess this is how we copy anything we need to 
    // ODD* p = &odd;
    // ODD newODD = *p;

    //maybe this is better to define new ODD
    ODD o;
    o.nLayers = odd->nLayers;

    o.layerSequence = malloc(o.nLayers * sizeof(Layer));

    //better to create new ODD and add layers to it like we thought orignally LOL

    int numLayers = odd->nLayers;

    //create new layer then we need to free the old one before replacing it with the new one we created
    Layer l;
    //change things in the layer using '.' e.g.
    l.width = odd->layerSequence[0].width;

    Layer currLayer = odd->layerSequence[0];

    State* livingStates = currLayer.leftStates.set;
    int livingCount = currLayer.leftStates.nStates;

    Transition* livingTransitions =  currLayer.transitions.set;
    int livingTransCount = currLayer.transitions.nTransitions;

    for (int i = 0; i < numLayers; i++) {
        l.leftStates.set = livingStates;
        l.leftStates.nStates = livingCount;
        l = deleteDeadTransitionsRight(&currLayer, &livingCount, livingStates, &livingTransCount, livingTransitions, l);
        l = deleteDeadStatesRight(&currLayer, &livingCount, livingStates, l);

        l.width = l.leftStates.nStates > l.rightStates.nStates ? l.leftStates.nStates : l.rightStates.nStates;

        o.layerSequence[i] = l;
    }

    for (int i = numLayers-1; i >= 0; i--) {
        l.rightStates.set = livingStates;
        l.rightStates.nStates = livingCount;
        l = deleteDeadTransitionsLeft(&currLayer, &livingCount, livingStates, &livingTransCount, livingTransitions, l);
        l = deleteDeadStatesLeft(&currLayer, &livingCount, livingStates, l);

        l.width = l.leftStates.nStates > l.rightStates.nStates ? l.leftStates.nStates : l.rightStates.nStates;

        o.layerSequence[i] = l;
    }

    //reset width to new width
    for(int i = 0; i < odd->nLayers; i++) {
        odd->width = odd->layerSequence[i].width > odd->width ? odd->layerSequence[i].width : odd->width;
    }

    //my part starts here the upper part is from daniel
    //we look at new type it is the kartisian product {Numbsymbol} x {left/right_state}
    //sort the transitions do we need to do that here ?? 
    void sortAllLeftTransitions(ODD* odd);
    void sortAllRightTransitions(ODD* odd);
    
    for (int i = 0; i < odd->nLayers; i++){
        l = minimizeLayer(&odd.layerSequence,i);    
        //get new trimmed Layer without the -1 states 
        
    }



    //reset width to new width
    for(int i = 0; i < odd->nLayers; i++) {
        odd->width = odd->layerSequence[i].width > odd->width ? odd->layerSequence[i].width : odd->width;
    }




    //free original ODD?
    free(odd);

    return &o;
}
