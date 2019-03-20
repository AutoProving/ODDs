// Copyright 2019 Andre Berge
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
//Parrallell
void sortAllRightTransitions(ODD* odd)
{
    for (int i = 0; i < odd->nLayers; i++)
    {
        sortRightTransitions(&(odd->layerSequence[i].transitions));
    }
}


void sortLeftTransitions(TransitionContainer* transitions)
{
    mergesort(transitions, 0, transitions->nTransitions - 1, true);
}

void sortRightTransitions(TransitionContainer* transitions)
{
    mergesort(transitions, 0, transitions->nTransitions - 1, false);
}
//Parallell her også.
void mergesort(TransitionContainer* transitions, int lo, int hi, bool sortLeft)
{
    if (lo >= hi) return;
    int mid =(lo + (hi-lo)/2); 
    mergesort(transitions, lo, mid, sortLeft);
    mergesort(transitions, mid+1, hi, sortLeft);
    
    merge(transitions, lo, mid, mid+1, hi, sortLeft);
}

void merge(TransitionContainer* transitions, int leftLo, int leftHi, int rightLo, int rightHi, bool sortLeft)
{   
    TransitionContainer* leftArr;
    //Should not be a pointer!
    //memcpy
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
