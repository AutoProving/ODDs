#include "odd.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
// Copyright 2019 YOUR NAME
// This file is licensed under MIT License, as specified in the file LISENSE located at the root folder of this repository.

//Union (Functions implemented in file union.c)

void unionLayers(Layer* layer1, Layer* layer2, Layer* result){

    //venstre
    StateContainer* leftStates2=&(layer2->leftStates);   //tilsvarer B'.left_states i notatene
    StateContainer* leftStates1=&(layer1->leftStates);    //tilsvarer B.left_states i notatene

    State* newLeftStates=malloc(sizeof(State)*(leftStates2->nStates)*(leftStates1->nStates));
    int sizeLeft=0;
    //hent states fra settet til B' og B
    for(int i=0; i<leftStates2->nStates; i++){
        for(int j=0; j<leftStates1->nStates; j++){
            State* state2=&(leftStates2->set[i]);
            State* state1=&(leftStates1->set[j]);

            State newState=((*state1) * (layer2->width)+(*state2));
            sizeLeft++;
            newLeftStates[i*j]=&newState;   

        }

    }

    StateContainer resultLeftStates={.nStates=sizeLeft,.set=newLeftStates};


    //høyre
    StateContainer* rightStates2=&(layer2->rightStates);
    StateContainer* rightStates1=&(layer1->rightStates);

    State* newRightStates=malloc(sizeof(State)*(rightStates2->nStates)*(rightStates1->nStates));
    int sizeRight=0;
    //hent states fra settet til B'
    for(int i=0; i<rightStates2->nStates; i++){
        for(int j=0; j<rightStates1->nStates; j++){
            State* state2=&(rightStates2->set[i]);
            State* state1=&(rightStates1->set[j]);

            State newState=((*state1) * (layer2->width)+state2);
            sizeLeft++;
            newRightStates[i*j]=&newState;   

        }
    }

    StateContainer resultRightStates={.nStates=sizeRight,.set=newRightStates};
    

    //initial
    StateContainer* initialStates2=&(layer2->initialStates);
    StateContainer* initialStates1=&(layer1->initialStates);

    State* newInitStates=malloc(sizeof(State)*(initialStates2->nStates)*(initialStates1->nStates));
    int sizeInit=0;
    //hent states fra settet til B'
    for(int i=0; i<initialStates2->nStates; i++){
        for(int j=0; j<initialStates1->nStates; j++){
            State* state2=&(initialStates2->set[i]);
            State* state1=&(initialStates1->set[j]);

            State newState=((*state1) * (layer2->width)+state2);
            sizeLeft++;
            newInitStates[i*j]=&newState;   

        }
    }

    StateContainer resultInitialStates={.nStates=sizeInit,.set=newInitStates};


//final er forskjellig fra intersection!!
//alphabetmap også???  kven veit

bool initialFlag=layer1->initialFlag;
bool finalFlag=layer1->finalFlag;

AlphabetMap map1=layer1->map;   //????????????






//FIKS FINAL OG TRANSITION



//leftStates2->set[leftLayer->nStates-1]

   //result->leftStates=
   //StateContainer resultLeftStates={.nStates=2,.set=};
}
void uniontODDs(ODD* odd1, ODD* odd2, ODD* odd);