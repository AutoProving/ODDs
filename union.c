#include "odd.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <omp.h>  
#include <string.h>
 //bruk denne i parallellkoden
// Copyright 2019 Nora Hobæk Hovland
// This file is licensed under MIT License, as specified in the file LISENSE located at the root folder of this repository.

//Union (Functions implemented in file union.c)


StateContainer* unionStates(StateContainer* state1, StateContainer* state2){
    
    int width=state2->set[state2->nStates-1]+1;
    StateContainer* result=malloc(sizeof(StateContainer));

    result->nStates=state1->nStates*state2->nStates;
    result->set=malloc(sizeof(State)*result->nStates);

    int index=0;
    for(int i=0; i<state1->nStates; i++){
        for(int j=0; j<state2->nStates; j++){
            result->set[index++] = state1->set[i]*width + state2->set[j];
        }
    }

    return result;
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

Layer* unionTransitions(Layer* layer1, Layer* layer2){
    TransitionContainer* transStates1=&(layer1->transitions);
    TransitionContainer* transStates2=&(layer2->transitions);
    int width1=layer2->leftStates.set[layer2->leftStates.nStates-1]+1;
    int width2=layer2->rightStates.set[layer2->rightStates.nStates-1]+1;
    
    int transCount=countTrans(layer1, layer2);
    Layer* result=malloc(sizeof(Layer));   

    result->transitions.set=malloc(sizeof(Transition)*transCount);
    result->transitions.nTransitions=transCount;

    int index=0;
    for(int i=0; i<transStates1->nTransitions; i++){
        for(int j=0; j<transStates2->nTransitions; j++){
            if(transStates1->set[i].a==transStates2->set[j].a){
             
                Transition newTrans;
                newTrans.s1=((transStates1->set[i].s1)) * width1+(transStates2->set[j].s1);
                newTrans.s2=((transStates1->set[i].s2))*width2+(transStates2->set[j].s2);
                newTrans.a=transStates1->set[i].a;
                
                result->transitions.set[index++]=newTrans;
               
            }
        }
    }
    return result;
}



Layer* unionLayers(Layer* layer1, Layer* layer2){

    Layer* result=malloc(sizeof(Layer));

    //left states
    StateContainer* leftStates1=&(layer1->leftStates);   
    StateContainer* leftStates2=&(layer2->leftStates);    
    unionStates(leftStates1,leftStates2);
   
    //right states
    StateContainer* rightStates1=&(layer1->rightStates);
    StateContainer* rightStates2=&(layer2->rightStates);
    unionStates(rightStates1,rightStates2);
   
    //initial states
    StateContainer* initialStates1=&(layer1->initialStates);
    StateContainer* initialStates2=&(layer2->initialStates);
    unionStates(initialStates1, initialStates2);
    
    assert(layer1->initialFlag==layer2->initialFlag);
    assert(layer1->finalFlag==layer2->finalFlag);

    result->initialFlag=layer1->initialFlag;   //bare fiks final og initial hvis true
    result->finalFlag=layer1->finalFlag;
  
    memcpy(&result->map,&layer1->map, sizeof(AlphabetMap));  //?????

    //width
    if(result->leftStates.nStates > result->rightStates.nStates){
        result->width=result->leftStates.nStates;
    }
    else{
        result->width=result->rightStates.nStates;
    }

    if(result->finalFlag){
        //final states ??????
        int width=layer2->rightStates.set[layer2->rightStates.nStates-1]+1;//right eller final??
        int index=0;      
        for(int s1=0;s1<layer1->rightStates.nStates;s1++){
            for(int s2=0;s2<layer2->rightStates.nStates;s2++){
                State* state1=findState(&layer1->finalStates, &rightStates1->set[s1]);
                State* state2=findState(&layer2->finalStates, &rightStates2->set[s2]);
                if(layer1->rightStates.set[s1]==*state1 || layer2->rightStates.set[s2]==*state2){
                    result->finalStates.set[index++]=layer1->rightStates.set[s1]*width + layer2->rightStates.set[s2];
                }
            }
        }
    }


    //transitions
    unionTransitions(layer1, layer2);
   // result->finalStates=layer1->finalStates;


   return result;
}


ODD* unionODDs(ODD* odd1, ODD* odd2){
    ODD* odd=malloc(sizeof(ODD));
    //assert(isCompleteODD(odd1));
    //assert(isCompleteODD(odd2));
    assert(odd1->nLayers==odd2->nLayers);
  
    odd->nLayers=odd1->nLayers;
    odd->layerSequence=malloc(sizeof(Layer)*odd->nLayers);
    odd->width=0;
    for(int i=0;i<odd->nLayers;i++){
        Layer* result=unionLayers(&odd1->layerSequence[i], &odd2->layerSequence[i]);
        odd->layerSequence[i]=*result;
        if(odd->width < result->width){
            odd->width=result->width;
        }
        free(result);
    }

    return odd;
}
