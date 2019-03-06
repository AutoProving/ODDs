#include "odd.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
// Copyright 2019 YOUR NAME
// This file is licensed under MIT License, as specified in the file LISENSE located at the root folder of this repository.

//Union (Functions implemented in file union.c)


void unionStates(StateContainer* state1, StateContainer* state2, StateContainer* result, int l2wth){
    result->nStates = state1->nStates*state2->nStates;
    result->set = malloc(sizeof(State)*result->nStates);

    int index=0;
    for(int i=0; i<state1->nStates; i++){
        for(int j=0; j<state2->nStates; j++){
            result->set[index++] = state1->set[i]*l2wth + state2->set[j];
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
    int length1=initialStates1->nStates;
    int length2=initialStates2->nStates;
    int sizeInit=0;

    //check if one of the layers has 0 initial states
    if(length1==0 && length2!=0){
        result->initialStates.nStates=length2;
        result->initialStates.set=malloc(sizeof(State)*length2);
        for(int i=0;i<length2;i++){
            State* state2=&(initialStates2->set[i]);
            
            State newState=(*state2);
            sizeInit++;
            newInitStates[i]=newState;
            result->initialStates->set= &(initialStates2->set[i]);
        }
        //StateContainer resultInitialStates={.nStates=sizeInit,.set=newInitStates};
        result->initialStates->nStates= sizeInit;
        retult->initialStates->set=newInitStates;
        //result->initialStates=resultInitialStates;
    }


    else if(length2==0 && length1!=0){
        State* newInitStates=malloc(sizeof(State)*(length1));
        for(int i=0;i<length1;i++){
            State* state1=&(initialStates1->set[i]);
            
            State newState=*state1 * (layer2->width);
            sizeInit++;
            newInitStates[i]=newState;
        }
        StateContainer resultInitialStates={.nStates=sizeInit,.set=newInitStates};
        result->initialStates=resultInitialStates;
    }

    else if(length1==0 && length2==0){
        result->initialStates=layer1->initialStates;    //tomt
    }
    else{
        unionStates(initialStates1, initialStates2, &result->initialStates, layer2->width);
    }
   
    result->initialFlag=layer1->initialFlag;
    result->finalFlag=layer1->finalFlag;
    result->map=layer1->map;

    //result->transitions=layer1->transitions;   //midlertidig
    result->width=layer1->width;                //midlettidig

    result->finalStates=layer1->finalStates;  //midlertidig



    //FIKS FINAL OG TRANSITION

    /*
    However, when considering Intersection and union, you should first count
    the number
    of pairs of transitions (t1.s1, t1.a, t1.s2) and (t2.s1,t2.a,t2.s2) for
    which t1.a=t2.a . Then
    you can allocate the appropriate space for the vector
    layer.transitions.set, and finally do something like

    For t1 in layer1.transitions.set
        for t2 in layer2.transitions.set
            add the transition  [(t1.s1, t2.s1),  t1.a,  (t1.s2,t2.s2)]  to
    layer.transitions.set IF t1.a = t2.a.

    */
    TransitionContainer* transStates2=&(layer2->transitions);
    TransitionContainer* transStates1=&(layer1->transitions);
    int transSize=0;
    int length10=transStates1->nTransitions;
    Transition* newTransitions=malloc(sizeof(Transition)*(transStates2->nTransitions)*(transStates1->nTransitions));
    result->transitions.set=malloc(sizeof(Transition)*(transStates2->nTransitions)*(transStates1->nTransitions));
    
    for(int i=0; i<transStates1->nTransitions; i++){
        for(int j=0; j<transStates2->nTransitions; j++){
            if(transStates1->set[i].a==transStates2->set[j].a){
                
                State* state1=&(transStates1->set[i].s1);  //tilsvarer i i notatene
                State* state11=&(transStates1->set[i].s2);     //tilsvarer j i notatene
                State* state2=&(transStates2->set[j].s1);      //tilsvarer i'
                State* state22=&(transStates2->set[j].s2);     //tilsvarer j'
                Transition newTrans1;
                newTrans1.s1=(*state1) * (layer2->width)+(*state2);
                newTrans1.s2=(*state11)*(layer2->width)+(*state22);
                newTrans1.a=transStates1->set[i].a;
                newTransitions[transSize]=newTrans1;
                transSize++;
            }

        }
    }
    TransitionContainer resultTrans={.nTransitions=transSize,.set=newTransitions};
    result->transitions=resultTrans;




//leftStates2->set[leftLayer->nStates-1]

   //result->leftStates=
   //StateContainer resultLeftStates={.nStates=2,.set=};
}
//void uniontODDs(ODD* odd1, ODD* odd2, ODD* odd);