// Copyright 2019  Konstantin Rygol
// This file is licensed under MIT License, as specified in the file LISENSE located at 
//the root folder of this repository.
#include <stdio.h>
#include <stdlib.h>
#include "odd.h"
#include <omp.h> //also add -fopenmp to compile path

//This file holds a function called collapse_ODD 
//It takes an ODD without dead states (checked forward and backward) and 
//collapses states whereever possible 

//use binary search 

ODD * collapseODD(ODD * odd){
    

    //create a new ODD and add layers to it
    int numLayers = odd->nLayers;

    //create new layer then free old one 
    Layer currLayer = odd->layerSequence[0];

    
    //minimize final layer sort final layer after right states should be 

    //sort after the left states 
    
    //go backwards through ODD    
    for(int i = numLayers-1; i <= 0; i--)
    {
        minimizeLayer(odd->layerSequence[i]);
    }


    //update right states accoarding to leftstates of layer+1, final state 
    //will not be updated
    for(int i = 0; i < numLayers-1; i++)
    {
       // #pragma omp parallel for
        for(int j = 0; j < odd->layerSequence[i].rightStates.nStates; j++)
        {
            odd->layerSequence[i].rightStates.set[j] = odd->layerSequence[i+1].leftStates.set[j]; 
        }
    }
    //reroute the transitions that point to the deleted right states to the states they collapsed to
    for(int i = numLayers-1; i >= 0; i--)
    {
        Layer layer= odd->layerSequence[i];
        //if state is negative reroute to the abs() of negative number
        for(int j = 0; j < layer.transitions.nTransitions; j++)
        {
            if(layer.transitions.set[j].s2 < 0){
                layer.transitions.set[j].s2 = -layer.transitions.set[j].s2; 
            }
        }
    }
    //build new ODD
    ODD o;
    //we keep the numbe of Layers constant
    o.nLayers = odd -> nLayers;
    o.layerSequence = malloc(o.nLayers * sizeof(Layer));

    Layer l;
    //copy only states that are != -1 and their transitions
    for(int i = 0; i < numLayers; i++)
    {
        //the other variables of the struct are no dealt with in this function
        
        livingStateLeft(&(odd->layerSequence[i]), &(l.leftStates));
        livingStateLeft(&(odd->layerSequence[i]), &(l.rightStates));
        livingStateLeft(&(odd->layerSequence[i]), &(l.transitions));
        
        l.width = l.leftStates.nStates > l.rightStates.nStates ? 
            l.leftStates.nStates : l.rightStates.nStates;
        //add the new layer to the new odd
        o.layerSequence[i] = l;
    }
    //reset width of ODD to new width of ODD
    for(int i = 0; i < odd->nLayers; i++) {
        o.width = o.layerSequence[i].width > o.width ? 
            o.layerSequence[i].width : o.width;
    }
    return &o;

}

void livingStateLeft(Layer * layer, StateContainer * livingStateLeft){
    int count = 0;
    for(int i = 0; i < layer->leftStates.nStates; i++)
    {
        if(layer->leftStates.set[i] >= 0){
            livingStateLeft->set[count] = layer->leftStates.set[i];
            count++;
        }
    }
    livingStateLeft->nStates=&count;
}

void livingStateRight(Layer * layer, StateContainer * livingStateRight){
    int count = 0;
    for(int i = 0; i < layer->rightStates.nStates; i++)
    {
        if(layer->rightStates.set[i] >= 0){
            livingStateRight->set[count] = layer->rightStates.set[i];
            count ++;
        }
    }
    livingStateRight->nStates = &count;
}

void livingTransition(Layer * layer, TransitionContainer * livingTransition){
    int count = 0;
    for(int i = 0; i < layer->transitions.nTransitions; i++)
    {
        if(layer->transitions.set[i].s1 != -1){
            livingTransition->set[count] = layer->transitions.set[i];
            count ++;
        }
    }
    livingTransition->nTransitions = &count;

}

Layer minimizeLayer(Layer layer){  
    //we minimize the left states and then return the new layer l  
    //we start with rightmost layer 
    //deletes unnecessary states and reroutes transitions
    //#pragma omp parallel for
    for(int j = 0; j < layer.leftStates.nStates;j++){  //assume states are sorted   
        State state_j = layer.leftStates.set[j];
        if (state_j >= 0){
            for(int k = j; k < layer.leftStates.nStates;j++){ 
                State state_k = layer.leftStates.set[k];
                //if two states have only transitions that have the  same keys and right 
                //states collapse   
                if (checkTypeLeft(layer,state_j,state_k)){
                    collapseLeft(layer,state_j,state_k);    
                }
            } 
        }
    }
    //this layer has just the left states and the transitions altered
    return layer;
}

Layer minimizeFinalLayer(Layer layer){
    //we minimze the final layer no need to collapse transitions 
    for(int i = 0; i < layer.rightStates.nStates; i++)
    {
        State state_j = layer.rightStates.set[i];
        for(int  j = 0; j < layer.rightStates.nStates; j++)
        {
            State state_k = layer.rightStates.set[j];
            if (checkTypeRight(layer,state_j,state_k)){
                collapseRight(layer,state_j,state_k);
            }

        }
        
    }
    
}

bool checkTypeLeft(Layer layer,State state_j,State state_k){
    //j < k is implied if states are sorted
    int start_j = findTransitionLeft(layer.transitions,state_j );
    int start_k = findTransitionLeft(layer.transitions,state_k );
    int i = 0;
    //for(int i = 0; i < indexTransitions[state_j+1]-indexTransitions[state_j]; i++)
    //{
    //while left state stays the same = ntrans for a state
    while (layer.transitions.set[start_j].s1 == layer.transitions.set[start_j+i+1].s1)
    { 
        NumSymbol key_j = layer.transitions.set[start_j+i].a;
        State state_right_j = layer.transitions.set[start_j+i].s2;
        
        NumSymbol key_k = layer.transitions.set[start_k+i].a;
        State state_right_k = layer.transitions.set[start_k+i].s2;
        
        //if at least of the pairs of key and right state is not equal 
        //then do not collapse
        if(key_j != key_k || state_right_j != state_right_k){
            return 0; //not collapse
        }
        i++;                
    }
    return 1;
}

bool checkTypeRight(Layer layer,State state_j,State state_k){
    //j < k is implied if states are sorted
    int start_j = findTransitionLeft(layer.transitions,state_j);
    int start_k = findTransitionLeft(layer.transitions,state_k);
    int i = 0;
    //for(int i = 0; i < indexTransitions[state_j+1]-indexTransitions[state_j]; i++)
    //{
    //while left state stays the same = ntrans for a state
    while( layer.transitions.set[start_j].s2 == layer.transitions.set[start_j+i+1].s2)
    { 
        NumSymbol key_j = layer.transitions.set[start_j+i].a;
        State state_left_j = layer.transitions.set[start_j+i].s1;
        
        NumSymbol key_k = layer.transitions.set[start_k+i].a;
        State state_left_k = layer.transitions.set[start_k+i].s1;
        
        //if at least of the pairs of key and right state is not equal 
        //then do not collapse
        if(key_j != key_k || state_left_j != state_left_k){
            return 0; //not collapse
        }
        i++;                
    }
    return 1;
}

void collapseRight(Layer layer, State state_i, State state_j){
    //sets the deleted state to -1 and deletes the transitions to the lower state
    if(state_i < state_j){ //to ensure that state_i < state_j 
        State tmp =  state_i;
        state_i = state_j;
        state_j = tmp;
        }
    //delete state j and say where it get rerouted to 
    layer.leftStates.set[state_j] = -state_i; 
    //collapse all transitions that start in state_j to the state_i
    //both have the same number of Transitions so we could also take state_j
    //we set the transition of the to be deleted state to -1
    int i = 0;
    int start_j = findTransitionLeft(layer.transitions,state_j);
    while(layer.transitions.set[start_j].s2 == layer.transitions.set[start_j+i].s2)
    //ntrans of that state
    //for(int i = 0; i <  indexTransitions[state_i+1]-indexTransitions[state_i] ; i++)
    {
        layer.transitions.set[start_j+i].s2 = -1;
        i++;
    }
       
}

void collapseLeft(Layer layer, State state_i, State state_j){
    //sets the deleted state to -1 and deletes the transitions to the lower state
    if(state_i < state_j){ //to ensure that state_i < state_j 
        State tmp =  state_i;
        state_i = state_j;
        state_j = tmp;
        }
    //delete state j and say where it get rerouted to 
    layer.leftStates.set[state_j] = -state_i; 
    //collapse all transitions that start in state_j to the state_i
    //both have the same number of Transitions so we could also take state_j
    //we set the transition of the to be deleted state to -1
    int i = 0;
    int start_j = findTransitionLeft(layer.transitions,state_j);
    while(layer.transitions.set[start_j].s1 == layer.transitions.set[start_j+i].s1)
    //ntrans of that state
    //for(int i = 0; i <  indexTransitions[state_i+1]-indexTransitions[state_i] ; i++)
    {
        layer.transitions.set[start_j+i].s1 = -1;
        i++;
    }
       
}


//returns the first transition of a state 
int findTransitionLeft(TransitionContainer transitions, State state){
    //Transition * findTransitionLeft(TransitionContainer transitions, State state){
    //finds the index of first occurence of a state in the sorted transitions list
    //returns then addres of this transition
    int left = -1;
    int right = transitions.nTransitions;

    while (right > left +1){
        int middle = (left+right)/2;
        //cout << "middle" << middle << endl;
        if (transitions.set[middle].s1 >= state){
            right = middle;
        }
        else{
            left = middle;
        }
    }
    if (right < transitions.nTransitions && transitions.set[right].s1 == state){
        return &(transitions.set[right]);
    }
    else{
        printf("state is not part of transition list \n");
        return &(transitions.set[right]);
    }

}

//returns the first transition of a state 
int findTransitionRight(TransitionContainer transitions, State state){
    //Transition * findTransitionLeft(TransitionContainer transitions, State state){
    //finds the index of first occurence of a state in the sorted transitions list
    //returns then addres of this transition
    int left = -1;
    int right = transitions.nTransitions;

    while (right > left +1){
        int middle = (left+right)/2;
        //cout << "middle" << middle << endl;
        if (transitions.set[middle].s2 >= state){
            right = middle;
        }
        else{
            left = middle;
        }
    }
    if (right < transitions.nTransitions && transitions.set[right].s2 == state){
        return &(transitions.set[right]);
    }
    else{
        printf("state is not part of transition list \n");
        return &(transitions.set[right]);
    }

}

