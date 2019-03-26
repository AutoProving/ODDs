// Copyright 2019 YOUR NAME
// This file is licensed under MIT License, as specified in the file LISENSE located at the root folder of this repository.
#include "odd.h"
#include <string.h>
#include <stdlib.h>


int  cantorPairing(int a, int b){
    return b+(((a+b)*(a+b+1))/2);
};

int max(int* list, int size){
    int max = list[0];
    for (int i = 1; i < size; ++i) {
       if (max<list[i]){
       	max = list[i];
       }
    }
	return max;
}

void tensorStates(StateContainer* stateContainer1, StateContainer* stateContainer2, StateContainer* result){

		int nStates = (stateContainer1->nStates)*(stateContainer2->nStates);
		State* set = malloc(sizeof(State)*nStates);
		for (int i = 0; i < stateContainer1->nStates; ++i) {
			for (int j = 0;  j < stateContainer2->nStates; ++j) {
				set[i*stateContainer1->nStates+j] = cantorPairing(stateContainer1->set[i], stateContainer2->set[j]);
			}
		}
		result->nStates = nStates;
		result->set = set;
}

void tensorEdgeStates(StateContainer* states1, StateContainer* edgeStates1, StateContainer* states2, StateContainer* edgeStates2, StateContainer* result){
	int a = max(states1->set, states1->nStates);
	int b =  max(states2->set, states2->nStates);
	if (a>b){
		int temp = a;
		a = b;
		b= temp;
	}
	int fullStateSize = cantorPairing(a,b)+1;
	int* countSortSet = malloc(sizeof(int)*fullStateSize);
	for (int i = 0; i <fullStateSize; ++i) {
		countSortSet[i] = 0;
	}
	int nStates = 0;

	for (int i = 0; i < edgeStates1->nStates; ++i) {
		for (int j = 0; j < states2->nStates; ++j) {
			int index = cantorPairing(edgeStates1->set[i],states2->set[j]);
			if (countSortSet[index]==0){
				countSortSet[index]+=1;
				nStates+=1;
			}
		}
	}

	for (int j = 0; j < edgeStates2->nStates; ++j) {
		for (int i = 0; i < states1->nStates; ++i) {
			int index = cantorPairing(states1->set[i],edgeStates2->set[j]);
			if (countSortSet[index]==0){
				countSortSet[index]+=1;
				nStates+=1;
			}
		}
	}

	State* set = malloc(sizeof(State)*nStates);

	int setIndex = 0;
	for (int i = 0; i < fullStateSize; ++i) {
		if (countSortSet[i]==1){
			set[setIndex] = i;
			setIndex+= 1;
		}
	}
	
	free(countSortSet);
	result->nStates = nStates;
	result->set = set;

}

Layer* tensorLayers(Layer* layer1, Layer* layer2) {
	Layer* result = malloc(sizeof(Layer));
	int newAlphabetSize = layer1->map.sizeAlphabet*layer2->map.sizeAlphabet;
	AlphabetMap map = {newAlphabetSize,malloc(sizeof(char*)*newAlphabetSize),malloc(sizeof(int)*newAlphabetSize)};
	for(int i = 0; i < layer1->map.sizeAlphabet; ++i){
		char* l1Symbol = layer1->map.N2S[i];
		int l1Length = strlen(l1Symbol);
		for(int j = 0; j < layer2->map.sizeAlphabet; ++j){
			char* l2Symbol = layer2->map.N2S[j];
			int l2Length = strlen(l2Symbol);
			int length = l1Length+l2Length;
			char* newSymbol = malloc(sizeof(char)*(length+2));
			strncpy(newSymbol,l1Symbol,l1Length);
			newSymbol[l1Length] ='|';
			strncpy(&newSymbol[l1Length+1],l2Symbol,l2Length+1);
			map.N2S[i*layer2->map.sizeAlphabet+j]=newSymbol;
			map.S2N[i*layer2->map.sizeAlphabet+j]=i*layer2->map.sizeAlphabet+j;
		}
	}

	result->map = map;

	tensorStates(&layer1->leftStates,&layer2->leftStates, &result->leftStates);
	tensorStates(&layer1->rightStates,&layer2->rightStates, &result->rightStates);

	result->width = result->leftStates.nStates>result->rightStates.nStates ? result->leftStates.nStates : result->rightStates.nStates;
	result->initialFlag = layer1->initialFlag || layer2->initialFlag;
	result->finalFlag = layer1->finalFlag || layer2->finalFlag;

	//could be reduced to a if statement and a 2 calls.
	if(result->initialFlag) {
		tensorEdgeStates(&layer1->leftStates, &layer1->initialStates,
						&layer2->leftStates, &layer2->initialStates,
						 &result->initialStates);
	} else{
		result->initialStates.nStates = 0;
		result->initialStates.set = NULL;
	}
	if(result->finalFlag) {
		tensorEdgeStates(&layer1->rightStates, &layer1->finalStates,
						 &layer2->rightStates, &layer2->finalStates,
						 &result->finalStates);
	} else{
		result->finalStates.nStates = 0;
		result->finalStates.set = NULL;
	}



	int nTransitions = layer1->transitions.nTransitions*layer2->transitions.nTransitions;
	Transition* set = malloc(sizeof(Transition)*nTransitions);
	for(int i = 0; i< layer1->transitions.nTransitions; ++i){
		Transition l1Trance = layer1->transitions.set[i];
		for(int j = 0; j< layer2->transitions.nTransitions; ++j){
			Transition l2Trace = layer2->transitions.set[j];
			State s1 = cantorPairing(l1Trance.s1, l2Trace.s1);
			NumSymbol  a = l1Trance.a*layer2->map.sizeAlphabet+l2Trace.a;
			State s2 = cantorPairing(l1Trance.s2, l2Trace.s2);
			Transition t = {s1,a,s2};
			set[i*layer2->transitions.nTransitions+j]= t;
		}
	}

	result->transitions.set = set;
	result->transitions.nTransitions = nTransitions;

	return result;

}
ODD* tensorODD(ODD* odd1, ODD* odd2){
	ODD *odd = malloc(sizeof(ODD));
	int nLayers = odd1->nLayers;
	Layer* layerSequence = malloc(sizeof(Layer)*nLayers);
	int width = 0;
	for (int i = 0; i < nLayers; ++i) {
		Layer* temp =  tensorLayers(&odd1->layerSequence[i],&odd2->layerSequence[i]);
		layerSequence[i] = *temp;
		free(temp);
		width = layerSequence[i].width > width ? layerSequence[i].width : width;
	}
	odd->nLayers=nLayers;
	odd->layerSequence = layerSequence;
	odd->width = width;
	return odd;
}

