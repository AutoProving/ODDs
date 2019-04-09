// Copyright 2019 Andre Berge, Ole Magnus Lie
// This file is licensed under MIT License, as specified in the file LISENSE located at the root folder of this repository.

#include <stdio.h>
#include "odd.h"
#include <stdlib.h>

void sortAllLeftTransitions(ODD* odd)
{
    for (int i = 0; i < odd->nLayers; i++)
    {
        sortLeftTransitions(&(odd->layerSequence[i].transitions));
    }
}

void sortAllRightTransitions(ODD* odd)
{
    for (int i = 0; i < odd->nLayers; i++)
    {
        sortRightTransitions(&(odd->layerSequence[i].transitions));
    }
}


void sortLeftTransitions(TransitionContainer* transitions)
{
    mergesortTransitions(transitions, 0, transitions->nTransitions - 1, true);
}

void sortRightTransitions(TransitionContainer* transitions)
{
    mergesortTransitions(transitions, 0, transitions->nTransitions - 1, false);
}

void mergesortTransitions(TransitionContainer* transitions, int lo, int hi, bool sortLeft)
{
    if (lo >= hi) return;
    int mid =(lo + (hi-lo)/2); 
    mergesortTransitions(transitions, lo, mid, sortLeft);
    mergesortTransitions(transitions, mid+1, hi, sortLeft);
    
    mergeTransitions(transitions, lo, mid, mid+1, hi, sortLeft);
}

void mergeTransitions(TransitionContainer* transitions, int leftLo, int leftHi, int rightLo, int rightHi, bool sortLeft)
{   
    TransitionContainer* leftArr;
    TransitionContainer* rightArr;
    leftArr = malloc(sizeof(TransitionContainer));
    rightArr = malloc(sizeof(TransitionContainer));
    leftArr->nTransitions = ((leftHi - leftLo) + 1);
    leftArr->set = (Transition *)malloc(leftArr->nTransitions * sizeof(Transition));
    rightArr->nTransitions = ((rightHi - rightLo) + 1);
    rightArr->set = (Transition *)malloc(rightArr->nTransitions * sizeof(Transition));
    
    int index = 0;
    for (int i = leftLo; i <= leftHi; i++) leftArr->set[index++] = transitions->set[i]; 
    index = 0;
    for (int i = rightLo; i <= rightHi; i++) rightArr->set[index++] = transitions->set[i]; 

    int firstArrPointer = 0;
    int secondArrPointer = 0;
    index = leftLo;
    
    while (firstArrPointer <= leftHi && secondArrPointer <= rightHi - rightLo && index <= rightHi)
    {   
        int comparison;
        if (sortLeft) comparison = leftArr->set[firstArrPointer].s1 - rightArr->set[secondArrPointer].s1;
        else comparison = leftArr->set[firstArrPointer].s2 - rightArr->set[secondArrPointer].s2;
        if (comparison == 0) comparison = leftArr->set[firstArrPointer].a - rightArr->set[secondArrPointer].a;  


        if (comparison <= 0) transitions->set[index++] = leftArr->set[firstArrPointer++];
        else transitions->set[index++] = rightArr->set[secondArrPointer++];
    }
    while (firstArrPointer <= leftHi && index <= rightHi)
    {
        transitions->set[index++] = leftArr->set[firstArrPointer++]; 
    }
    while (secondArrPointer <= rightHi - rightLo && index <= rightHi)
    {
        transitions->set[index++] = rightArr->set[secondArrPointer++];
    }

    free(leftArr);
    free(rightArr);
}

/* SORT STATES */

void sortAllLeftStates(ODD *odd)
{
    for (int i = 0; i < odd->nLayers; i++)
    {
        sortStates(&(odd->layerSequence[i].leftStates));
    }  
}

void sortAllRightStates(ODD *odd)
{
    for (int i = 0; i < odd->nLayers; i++)
    {
        sortStates(&(odd->layerSequence[i].rightStates));
    }  
}

void sortStates(StateContainer *states)
{
    mergesortStates(states, 0, states->nStates - 1);
}

void mergesortStates(StateContainer *states, int lo, int hi)
{
    if (lo >= hi) return;
    int mid =(lo + (hi-lo)/2); 
    mergesortStates(states, lo, mid);
    mergesortStates(states, mid+1, hi);
    
    mergeStates(states, lo, mid, mid+1, hi);
}

void mergeStates(StateContainer* states, int leftLo, int leftHi, int rightLo, int rightHi)
{   
    StateContainer* leftArr;
    StateContainer* rightArr;
    leftArr = malloc(sizeof(StateContainer));
    rightArr = malloc(sizeof(StateContainer));
    leftArr->nStates = ((leftHi - leftLo) + 1);
    leftArr->set = (State *)malloc(leftArr->nStates * sizeof(State));
    rightArr->nStates = ((rightHi - rightLo) + 1);
    rightArr->set = (State *)malloc(rightArr->nStates * sizeof(State));
    
    int index = 0;
    for (int i = leftLo; i <= leftHi; i++) leftArr->set[index++] = states->set[i]; 
    index = 0;
    for (int i = rightLo; i <= rightHi; i++) rightArr->set[index++] = states->set[i]; 

    int firstArrPointer = 0;
    int secondArrPointer = 0;
    index = leftLo;
    
    while (firstArrPointer <= leftHi && secondArrPointer <= rightHi - rightLo && index <= rightHi)
    {   
        int comparison = leftArr->set[firstArrPointer] - rightArr->set[secondArrPointer];

        if (comparison <= 0) states->set[index++] = leftArr->set[firstArrPointer++];
        else states->set[index++] = rightArr->set[secondArrPointer++];
    }
    while (firstArrPointer <= leftHi && index <= rightHi)
    {
        states->set[index++] = leftArr->set[firstArrPointer++]; 
    }
    while (secondArrPointer <= rightHi - rightLo && index <= rightHi)
    {
        states->set[index++] = rightArr->set[secondArrPointer++];
    }

    free(leftArr);
    free(rightArr);
}
