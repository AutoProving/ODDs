// Copyright 2019  Konstantin Rygol
// This file is licensed under MIT License, as specified in the file LISENSE located at 
//the root folder of this repository.
#include <stdio.h>
#include <stdlib.h>
#include "odd.h"
#include <omp.h> //also add -fopenmp to compile path
#include "debugTools.h" 
//This file holds a function called collapse_ODD 
//It takes an ODD without dead states (checked forward and backward) and 
//collapses states whereever possible 

ODD * collapseODD(ODD * odd);

void livingStateLeft(Layer * layer, StateContainer * livingStateLeft);

void livingStateRight(Layer * layer, StateContainer * livingStateRight);

void livingTransition(Layer * layer, TransitionContainer * livingTransition);

bool checkTypeLeft(Layer layer,State state_j,State state_k);

bool checkTypeRight(Layer layer,State state_j,State state_k);

void minimizeLayer(Layer layer);

void minimizeFinalLayer(Layer layer);

void collapseRight(Layer layer, State state_i, State state_j);

void collapseLeft(Layer layer, State state_i, State state_j);

Transition * findTransitionLeft(TransitionContainer transitions, State state);

Transition * findTransitionRight(TransitionContainer transitions, State state);


int main(int argc,char **argv){
    ODD odd;
    
    readODD("ODD-ToMinimize.txt",&odd); 
    showODD(&odd);
    //printf("Before collapsing ODD \n");   
    ODD * new_odd = collapseODD(&odd);
    fprintf(stderr,"collapsing is done\n");
    printODD("ODD-Mysave.txt",new_odd);
    showODD(new_odd);
    return 0; 
}


ODD * collapseODD(ODD * odd){
    fprintf(stderr,"start collapsing ODD \n");
    //printf("start collapsing ODD");
    //create a new ODD and add layers to it
    int numLayers = odd->nLayers;
    //minimize final layer sort final layer after right states should be 
    fprintf(stderr,"Before final layer \n");

    void sortAllLeftTransitions(ODD* odd);
    minimizeFinalLayer(odd->layerSequence[numLayers-1]);
    //showLayer(&(odd->layerSequence[numLayers-1]));
    fprintf(stderr,"Minimzed final layer \n");
    //sort after the left states 
    
    //go backwards through ODD    
    for(int i = numLayers-1; i >= 0; i--)
    {
        //minize the left states                    //this is faulty
        fprintf(stderr,"Number of left states %d \n",odd->layerSequence[i].leftStates.nStates);
        minimizeLayer(odd->layerSequence[i]);

        fprintf(stderr,"minimized left states in layer: %d \n",i);
        //collapse the right states accoardingly, The left states of Layer[i] 
        //corresponding to the right states of Layer[i-1]
        
        if (i != 0){ 
            for(int j = 0; j < odd->layerSequence[i-1].rightStates.nStates; j++)
            {
                odd->layerSequence[i-1].rightStates.set[j] 
                    = odd->layerSequence[i].leftStates.set[j]; 
            }
        }
        
        //reroute the transitions that point to the deleted right states 
        //to the states they collapsed to
        //if state is negative reroute to the abs() of negative number and shift by 1 
        //maybe the transitions should be sorted after right states ?? 

        
        for(int j = 0; j < odd->layerSequence[i].transitions.nTransitions; j++)
        {
            //we need the state to a transition or sorted transitions after the right states
            if(odd->layerSequence[i].rightStates.set[j] < 0)
            {
                odd->layerSequence[i].transitions.set[j].s2 
                    = -(odd->layerSequence[i].transitions.set[j].s2-1); 
            }
        }
        
        fprintf(stderr,"Rerouted Transitions in layer: %d \n",i);
    //    fprintf(stderr,"Minimzed layer %d \n",i);
    }
   
    //build new ODD
    fprintf(stderr,"Build new odd \n");
    ODD * o = malloc(sizeof(ODD));
    o->width = odd->width;
  
    //we keep the number of Layers constant
    o->nLayers = odd -> nLayers;
    o->layerSequence = malloc(o->nLayers * sizeof(Layer));

    Layer l;
    //copy only states that are != -1 and their transitions
    for(int i = 0; i < numLayers; i++)
    {
        //the other variables of the struct are no dealt with in this function   
        livingStateLeft(&(odd->layerSequence[i]), &(l.leftStates));
        livingStateRight(&(odd->layerSequence[i]), &(l.rightStates));
        livingTransition(&(odd->layerSequence[i]), &(l.transitions));
        l.width = l.leftStates.nStates > l.rightStates.nStates ? 
            l.leftStates.nStates : l.rightStates.nStates;
        //coopy the other attributes from the old ODD
     //   TransitionContainer test_container;
    //test_container.set = (Transition *) malloc(10 * sizeof(Transition));
        l.map.N2S = malloc(sizeof(char*) * odd->layerSequence[i].map.sizeAlphabet); 
        l.map.S2N = malloc(sizeof(int) * odd->layerSequence[i].map.sizeAlphabet); 

        l.map.N2S = odd->layerSequence[i].map.N2S;
        l.map.S2N = odd->layerSequence[i].map.S2N;
        l.map.sizeAlphabet = odd->layerSequence[i].map.sizeAlphabet;
        l.initialFlag = odd->layerSequence[i].initialFlag;
        l.finalFlag = odd->layerSequence[i].finalFlag;
        l.initialStates.nStates = odd->layerSequence[i].initialStates.nStates;
        l.finalStates.nStates = odd->layerSequence[i].finalStates.nStates;

        if (i == numLayers-1){
            l.finalStates.set = l.rightStates.set;
        }
        if (i == 0){
            l.initialStates.set = l.leftStates.set;
        }

        //add the new layer to the new odd
        o->layerSequence[i] = l;
    }

    o->layerSequence[0].initialStates=o->layerSequence[0].leftStates;
    o->layerSequence[numLayers-1].finalStates=o->
        layerSequence[numLayers-1].rightStates;

    //reset width of ODD to new width of ODD
    for(int i = 0; i < odd->nLayers; i++) {
        o->width = o->layerSequence[i].width > o->width ? 
            o->layerSequence[i].width : o->width;
    }
    printf("The width of the new ODD is %d", o->width);
    fprintf(stderr,"This is the minimized ODD \n");
    return o;//return o;

}

void minimizeLayer(Layer layer){  
    //we minimize the left states and then return the new layer l  
    //we start with rightmost layer 
    //deletes unnecessary states and reroutes transitions
    //#pragma omp parallel for
  //  fprintf(stderr,"Number of left states %d \n",layer.leftStates.nStates);
    for(int j = 0; j < layer.leftStates.nStates;j++){  //assume states are sorted   
        State state_j = layer.leftStates.set[j];
        if (state_j >= 0){
            for(int k = j+1; k < layer.leftStates.nStates;k++){ //added +1 no point
                //in chekcing itelself 
                State state_k = layer.leftStates.set[k];
                fprintf(stderr,"j: %d, k: %d, state_j: %d, state_k: %d \n" , j,k,state_j,state_k);
                //if two states have only transitions that have the  same keys and right 
                //states collapse   
                if (checkTypeLeft(layer,state_j,state_k)){
                     fprintf(stderr,"check TypeLeft worked for state_j:%d state_k:%d\n",
                        state_j,state_k);

                    collapseLeft(layer,state_j,state_k);    
                }
            } 
        }
    }
    //this layer has just the left states and the transitions altered
    //return layer;
}

void minimizeFinalLayer(Layer layer){
    //we minimze the final layer no need to collapse transitions 
    fprintf(stderr,"Start minimize final layer \n");
    for(int i = 0; i < layer.rightStates.nStates; i++)
    {
        State state_j = layer.rightStates.set[i];
        for(int  j = i+1; j < layer.rightStates.nStates; j++)
        {
            State state_k = layer.rightStates.set[j];
            fprintf(stderr,"i: %d, j: %d ,state_j %d, state_k %d \n",i,j,state_j,state_k);
            if (checkTypeRight(layer,state_j,state_k)){
                fprintf(stderr,"check TypeRight worked for state_j:%d state_k:%d\n",
                        state_j,state_k);
                collapseRight(layer,state_j,state_k);
            }

        }
        
    }
    
}

bool checkTypeLeft(Layer layer,State state_j,State state_k){
    //j < k is implied if states are sorted
    Transition * trans_j = findTransitionLeft(layer.transitions,state_j);
    Transition * trans_k = findTransitionLeft(layer.transitions,state_k);
    int i = 0;
    
   // fprintf(stderr,"Entered checkTypeLeft with start_j %d, start_k %d \n",start_j,start_k);

    //while left state stays the same = ntrans for a state
    while ((trans_j+i)->s1 == (trans_j+i+1)->s1)
    { 
        NumSymbol key_j = (trans_j+i)->a;
        State state_right_j = (trans_j+i)->s2;
        
        NumSymbol key_k = (trans_k+i)->a;
        State state_right_k = (trans_k+i)->s2;
     
        //fprintf(stderr,"start_j: %d , start_k: %d, i: %d",start_j,start_k,i);
        //fprintf(stderr,"key_j %d, state_r_j %d, key_k %d, state_r_k %d \n",
        //        key_j,state_right_j,key_k,state_right_k);  
        //if at least of the pairs of key and right state is not equal 
        //then do not collapse
        if(key_j != key_k || state_right_j != state_right_k){
            return 0; //not collapse
        }
        i++;                
    }
    return 1; //collapse
}

bool checkTypeRight(Layer layer,State state_j,State state_k){
    //j < k is implied if states are sorted
    Transition * trans_j = findTransitionLeft(layer.transitions,state_j);
    Transition * trans_k = findTransitionLeft(layer.transitions,state_k);
    
    fprintf(stderr, "trans_j %d %d %d \n", trans_j->s1, trans_j->a, trans_j->s2);
    fprintf(stderr, "trans_k %d %d %d \n", trans_k->s1, trans_k->a, trans_k->s2);
    showLayer(&layer);
    int i = 0;
    //while right state stays the same = ntrans for a state
    
    while ((trans_j+i)->s1 == (trans_j+i+1)->s1)
    { 
        NumSymbol key_j = (trans_j+i)->a;
        State state_right_j = (trans_j+i)->s2;
        
        NumSymbol key_k = (trans_k+i)->a;
        State state_right_k = (trans_k+i)->s2;
        fprintf(stderr,"i: %d , key_j %d, state_r_j %d, key_k %d, state_r_k %d \n",
                i,key_j,state_right_j,key_k,state_right_k);  
        
        //if at least of the pairs of key and right state is not equal 
        //then do not collapse
        if(key_j != key_k || state_right_j != state_right_k){
            return 0; //not collapse
        }
        i++;                
    }
    
    fprintf(stderr,"Will collapse set of Final states\n");
    return 1;
}

void collapseLeft(Layer layer, State state_i, State state_j){
    //sets the deleted state to -(upperstate+1) the transitions to the lower state will be deleted
    if(state_i > state_j){ //to ensure that state_i < state_j 
        State tmp =  state_i;
        state_i = state_j;
        state_j = tmp;
        }
    //delete state j and say where it get rerouted to save it as negative-1 to account for state 0 
    layer.leftStates.set[state_j] = -(state_i+1); 
    //collapse all transitions that start in state_j to the state_i
    //both have the same number of Transitions so we could also take state_j
    //we set the transition of the to be deleted state to -1
    int i = 0;
    Transition * trans_j = findTransitionLeft(layer.transitions,state_j);
    //buffer the left state that is to be overwritten
    int buff = trans_j->s1;
    int buff_incr = (trans_j+i)->s1;
    while(buff == buff_incr)
    {
        buff_incr = (trans_j+i)->s1;
        (trans_j+i)->s1 = -1;
        i++;
    }
       
}
void collapseRight(Layer layer, State state_i, State state_j){
    //sets the deleted state to -1 and deletes the transitions to the lower state
    if(state_i > state_j){ //to ensure that state_i < state_j 
        State tmp =  state_i;
        state_i = state_j;
        state_j = tmp;
        }
    //delete state j and say where it get rerouted to save it as negative-1 to account for state 0 
    layer.rightStates.set[state_j] = -(state_i+1); 
    //collapse all transitions that start in state_j to the state_i
    //both have the same number of Transitions so we could also take state_j
    //we set the transition of the to be deleted state to -1
    int i = 0;
    Transition * trans_j = findTransitionRight(layer.transitions,state_j);
    //buffer the left state that is to be overwritten
    int buff = trans_j->s2;
    int buff_incr = (trans_j+i)->s2;
    while(buff == buff_incr)
    {
        buff_incr = (trans_j+i)->s2;
        //maybe we do not have to collapse transitions ? 
        //(trans_j+i)->s1 = -1;
        i++;
    }     
    
}

//returns the first transition of a state 
Transition * findTransitionLeft(TransitionContainer transitions, State state){
    //Transition * findTransitionLeft(TransitionContainer transitions, State state){
    //finds the index of first occurence of a state in the sorted transitions list
    int left = -1;
    int right = transitions.nTransitions;

    while (right > left +1){
        int middle = (left+right)/2;
        //cout << "middle" << middle << endl;
        if (transitions.set[middle].s1 >= state){
            right = middle;
        }
        else {
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
Transition * findTransitionRight(TransitionContainer transitions, State state){
    //Transition * findTransitionLeft(TransitionContainer transitions, State state){
    //finds the index of first occurence of a state in the sorted transitions list  
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

void livingStateLeft(Layer * layer, StateContainer * livingStateLeft){
    int count = 0;
    for(int i = 0; i < layer->leftStates.nStates;  i++)
    {
        if(layer->leftStates.set[i] >= 0) 
            count++;
    }
    livingStateLeft->set = malloc(count * sizeof(State));
    livingStateLeft->nStates = count;
    count = 0;
    for(int i = 0; i < layer->leftStates.nStates; i++)
    {
        if(layer->leftStates.set[i] >= 0){
            livingStateLeft->set[count] = layer->leftStates.set[i];
            count++;
        }
    }
}

void livingStateRight(Layer * layer, StateContainer * livingStateRight){
    int count = 0;
    for(int i = 0; i < layer->leftStates.nStates;  i++)
    {
        if(layer->rightStates.set[i] >= 0)
            count++;
    }
    livingStateRight->set = malloc(count * sizeof(State));
    livingStateRight->nStates=count;
    count = 0;
    for(int i = 0; i < layer->rightStates.nStates; i++)
    {
        if(layer->rightStates.set[i] >= 0){
            livingStateRight->set[count] = layer->rightStates.set[i];
            count ++;
        }
    }
}

void livingTransition(Layer * layer, TransitionContainer * livingTransition){
    int count = 0;
    for(int i = 0; i < layer->transitions.nTransitions;  i++)
    {
        if(layer->transitions.set[i].s1  >= 0)
            count++;
    }
    livingTransition->set = malloc(count * sizeof(Transition));
    livingTransition->nTransitions=count;
    count = 0;
    for(int i = 0; i < layer->transitions.nTransitions; i++)
    {
        if(layer->transitions.set[i].s1  >= 0){
            livingTransition->set[count] = layer->transitions.set[i];
            count ++;
        }
    }
}
