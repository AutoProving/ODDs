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
    //the new ODD
    ODD o;
    //we keep the numbe of Layers constant
    o.nLayers = odd -> nLayers;
    o.layerSequence = malloc(o.nLayers * sizeof(Layer));

    //create a new ODD and add layers to it
    int numLayers = odd->nLayers;

    //create new layer then free old one 
    Layer currLayer = odd->layerSequence[0];
    //local
    //For performance reasons we need an array that points always to the first transition of a 
    //state. 
    int * indexTransitions = malloc((odd -> width)*sizeof(int)); 
    //go backwards through ODD
    for(int i = numLayers-1; i <= 0; i--)
    {
        minimizeLayer(odd->layerSequence[i],indexTransitions);
    }
    //update right states accoarding to leftstates of layer+1, final state 
    //will not be updated
    for(int i = 0; i < numLayers-1; i++)
    {
        #pragma omp parallel for
        for(int j = 0; j < odd->layerSequence[i].rightStates.nStates; j++)
        {
            odd->layerSequence[i].rightStates.set[j] = odd->layerSequence[i+1].leftStates.set[j]; 
        }
    }
    //reroute the transitions that point to the delted right states to the states they collapsed to
    //build new ODD
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
        if(layer->leftStates.set[i] != -1){
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
        if(layer->rightStates.set[i] != -1){
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

void CalculateIndexTransitions(TransitionContainer TransitionCon,int * indexTransitions){
    //array that points always to the first transition of a state. Assume sorted transitions 
    indexTransitions[0] = 0;
    int counter = 0;
    int index = 1;

    for(int i = 1; i < TransitionCon.nTransitions; i++)
    {
        if (TransitionCon.set[i-1].s1 != TransitionCon.set[i].s1){
            indexTransitions[index] += counter;
            counter = 0;
            index++;
        }
    counter++;
    }
}

Layer minimizeLayer(Layer layer,int * indexTransitions){  
    //we minimize the left states and then return the new layer l  
    //we start with rightmost layer 
    //deletes unnecessary states and reroutes transitions
    //#pragma omp parallel for
    for(int j = 0; j < layer.leftStates.nStates;j++){  //assume states are sorted   
        State state_j = layer.leftStates.set[j];
        if (state_j != -1){
            for(int k = j; k < layer.leftStates.nStates;j++){ //not sure about that ??
                State state_k = layer.leftStates.set[k];
                //if two states have only transitions that have the  same keys and right 
                //states collapse   
                if (checkTypeLeft(layer,state_j,state_k,indexTransitions)){
                    collapseLeft(layer,state_j,state_k,indexTransitions);   //where do we put right collapse ?  
                }
            } 
        }
    }
    //this layer has just the left states and the transitions altered
    return layer;
}

//takes tranitions not states as we also need NumSymbol
bool checkTypeLeft(Layer layer,State state_j,State state_k,int * indexTransitions){
    //j < k is implied if states are sorted
    for(int i = 0; i < indexTransitions[state_j+1]-indexTransitions[state_j]; i++)
    {
        NumSymbol key_j = layer.transitions.set[state_j].a;
        State state_right_j = layer.transitions.set[state_j].s2;
        for(int j = 0; j < indexTransitions[state_k+1]-indexTransitions[state_k]; j++)
        {
            NumSymbol key_k = layer.transitions.set[state_k].a;
            State state_right_k = layer.transitions.set[state_k].s2;
            //if at least one kartesian product of key and right state is not equal 
            //then do not collapse
            if(key_j != key_k && state_right_j != state_right_k){
                return 0; //not collapse

            }
            
        }        
    }
    return 1;
}
    
void collapseLeft(Layer layer, State state_i, State state_j, int * indexTransitions){
    //sets the deleted state to -1 and reroutes the Transitions to the lower state
    if(state_i < state_j){ //to ensure that state_i < state_j 
        State tmp =  state_i;
        state_i = state_j;
        state_j = tmp;
        }
    //delete state j 
    layer.leftStates.set[state_j] = -1;
    //collapse all transitions that start in state_j to the state_i
    //both have the same number of Transitions so we could also take state_j
    //we set the transition of the to be deleted state to -1
    for(int i = 0; i <  indexTransitions[state_i+1]-indexTransitions[state_i] ; i++)
    {
        layer.transitions.set [indexTransitions[state_j]+i].s1 
            = -1;
    }
       
}

/*
//original version after matheus pseudo code
void collapseLeft(Layer layer, State state_i, State state_j, int * indexTransitions){
    //sets the deleted state to -1 and reroutes the Transitions to the lower state
    if(state_i < state_j){ //to ensure that state_i < state_j 
        State tmp =  state_i;
        state_i = state_j;
        state_j = tmp;
        }
    //delete state j 
    layer.leftStates.set[state_j] = -1;
    //collapse all transitions that start in state_j to the state_i
    //both have the same number of Transitions so we could also take state_j
    //why do we do this ??? should it not be the same transition ?? 
    for(int i = 0; i <  indexTransitions[state_i+1]-indexTransitions[state_i] ; i++)
    {
        layer.transitions.set.s1[indexTransitions[state_j]+i] 
            = layer.transitions.set.s1[indexTransitions[state_i]+i];
    }
       
}
*/
//returns the first transition of a state 

Transition findTransitionLeft(TransitionContainer transitions, State state){
    //finds the first occurence of a state in the sorted transitions list
    int left = -1;
    int right = transitions.nTransitions;

    while (right < left +1){
        int middle = (left+right)/2;
        if (transitions.set[middle].s1 >= state){
            right = middle;
        }
        else{
            left = middle;
        }
    }
    if (right < transitions.nTransitions && transitions.set[right].s1 == state){
        return transitions.set[right];
    }
    else{
        printf("state is not part of transition list \n");
        return transitions.set[right];
    }


}

