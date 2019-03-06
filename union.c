#include "odd.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
// Copyright 2019 Nora Hobæk Hovland
// This file is licensed under MIT License, as specified in the file LISENSE located at the root folder of this repository.

//Union (Functions implemented in file union.c)


void unionStates(StateContainer* state1, StateContainer* state2, StateContainer* result, int l2wth){
    result->nStates = state1->nStates*state2->nStates;
    result->set = malloc(sizeof(State)*result->nStates);

    int length1=state1->nStates;
    int length2=state2->nStates;

    //check if one of the layers has 0 states
    if(length1==0 && length2!=0){                       //state1 has no states
        result->nStates=length2;
        result->set=malloc(sizeof(State)*length2);
        for(int i=0;i<length2;i++){
            result->set= &(state2->set[i]);
        }
    }

    else if(length2==0 && length1!=0){                  //state2 has no states
        result->nStates=length1;
        result->set=malloc(sizeof(State)*length1);
        for(int i=0;i<length1;i++){
            result->set= &(state1->set[i]);
        }
    }

    else if(length1==0 && length2==0){                  // both are empty
        result=state1;    
    }
    
    else{                                               //both statecontainers has states
        int index=0;
        for(int i=0; i<state1->nStates; i++){
            for(int j=0; j<state2->nStates; j++){
                result->set[index++] = state1->set[i]*l2wth + state2->set[j];
            }
        }
    }
}


int countTrans(Layer* layer1, Layer* layer2){
    int count=0;
    for(int i=0; i<layer1->transitions.nTransitions; i++){
        for(int j=0; j<layer2->transitions.nTransitions; j++){
            if(layer1->transitions.set[i].a==layer2->transitions.set[j].a){
                count++;
            }
        }
    }
    return count;
}

void unionTransitions(Layer* layer1, Layer* layer2, Layer* result){
    TransitionContainer* transStates1=&(layer1->transitions);
    TransitionContainer* transStates2=&(layer2->transitions);

    int n1=transStates1->nTransitions;
    int n2=transStates2->nTransitions;
    int transCount=countTrans(layer1, layer2);
    //Transition* newTransitions=malloc(sizeof(Transition)*n2*n1);
    result->transitions.set=malloc(sizeof(Transition)*transCount);
    result->transitions.nTransitions=transCount;

    int index=0;
    for(int i=0; i<transStates1->nTransitions; i++){
        for(int j=0; j<transStates2->nTransitions; j++){
            if(transStates1->set[i].a==transStates2->set[j].a){
             
                Transition newTrans;
                newTrans.s1=((transStates1->set[i].s1)) * (layer2->width)+(transStates2->set[j].s1);
                newTrans.s2=((transStates1->set[i].s2))*(layer2->width)+(transStates2->set[j].s2);
                newTrans.a=transStates1->set[i].a;
                
                result->transitions.set[index++]=newTrans;
               
            }
        }
    }
}



void unionLayers(Layer* layer1, Layer* layer2, Layer* result){
    //left states
    StateContainer* leftStates1=&(layer1->leftStates);   
    StateContainer* leftStates2=&(layer2->leftStates);    
    unionStates(leftStates1,leftStates2, &result->leftStates, layer2->width);
   
    //right states
    StateContainer* rightStates1=&(layer1->rightStates);
    StateContainer* rightStates2=&(layer2->rightStates);
    unionStates(rightStates1,rightStates2, &result->rightStates, layer2->width);
   
    //initial states
    StateContainer* initialStates1=&(layer1->initialStates);
    StateContainer* initialStates2=&(layer2->initialStates);
    unionStates(initialStates1, initialStates2, &result->initialStates, layer2->width);
    
    result->initialFlag=layer1->initialFlag;
    result->finalFlag=layer1->finalFlag;
    result->map=layer1->map;

    if(result->leftStates.nStates > result->rightStates.nStates){
        result->width=result->leftStates.nStates;
    }
    else{
        result->width=result->rightStates.nStates;
    }

                  

    result->finalStates=layer1->finalStates;  // FIKS DENNA

    unionTransitions(layer1, layer2, result);
}


void uniontODDs(ODD* odd1, ODD* odd2, ODD* odd){
    odd->nLayers=odd1->nLayers;
    odd->layerSequence=malloc(sizeof(Layer)*odd->nLayers);
    odd->width=0;
    for(int i=0;i<odd->nLayers;i++){
        Layer result;
        unionLayers(&odd1->layerSequence[i], &odd2->layerSequence[i], &result);
        odd->layerSequence[i]=result;
        if(odd->width < result.width){
            odd->width=result.width;
        }
    }
}