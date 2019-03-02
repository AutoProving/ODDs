// Copyright 2019 YOUR NAME
// This file is licensed under MIT License, as specified in the file LISENSE located at the root folder of this repository.
#include "odd.h"
#include <string.h>
#include <stdlib.h>
void tensorLayers(Layer* layer1, Layer layer2, Layer* result){
	int newAlphabetSize = layer1->map.sizeAlphabet*layer2.map.sizeAlphabet;
	AlphabetMap map = {newAlphabetSize,malloc(sizeof(char*)*newAlphabetSize),malloc(sizeof(int)*newAlphabetSize)};
	for(int i = 0; i < layer1->map.sizeAlphabet; ++i){
		char* l1Symbol = layer1->map.N2S[i];
		int l1Length = strlen(l1Symbol);
		for(int j = 0; j < layer2.map.sizeAlphabet; ++j){
			char* l2Symbol = layer2.map.N2S[j];
			int l2Length = strlen(l2Symbol);
			int length = l1Length+l2Length;
			char* newSymbol = malloc(sizeof(char)*(length+1));
			strncpy(newSymbol,l1Symbol,l1Length);
			strncpy(&newSymbol[l1Length],l2Symbol,l2Length+1);
			map.N2S[i*layer2.map.sizeAlphabet+j]=newSymbol;
			map.S2N[i*layer2.map.sizeAlphabet+j]=i*layer2.map.sizeAlphabet+j;
		}
	}

	result->map = map;

	//Left and right set could probably be reduced a single loop in call with a 3 layers as args.

	{

		int nStates = layer1->leftStates.nStates*layer2.leftStates.nStates;
		State* set = malloc(sizeof(State)*nStates);
		for (int i = 0; i < layer1->leftStates.nStates; ++i) {
			for (int j = 0;  j < layer2.leftStates.nStates; ++j) {
				set[i*layer1->leftStates.nStates+j] = i*layer1->leftStates.nStates+j;
			} 
		}
		result->leftStates.nStates = nStates;
		result->leftStates.set = set;
		
	}
	
	{

		int nStates = layer1->rightStates.nStates*layer2.rightStates.nStates;
		State* set = malloc(sizeof(State)*nStates);
		for (int i = 0; i < layer1->rightStates.nStates; ++i) {
			for (int j = 0;  j < layer2.rightStates.nStates; ++j) {
				set[i*layer1->rightStates.nStates+j] = i*layer1->rightStates.nStates+j;
			} 
		}
		result->rightStates.nStates = nStates;
		result->rightStates.set = set;
		
	}


	result->width = result->leftStates.nStates>result->rightStates.nStates ? result->leftStates.nStates : result->rightStates.nStates;
	result->initialFlag = layer1->initialFlag || layer2.initialFlag;
	result->finalFlag = layer1->finalFlag || layer2.finalFlag;

	//could be reduced to a if statement and a 2 calls.

    {
        int fullStateSize = layer1->leftStates.nStates*layer2.leftStates.nStates;
        int* countSortSet = malloc(sizeof(int)*fullStateSize);
		for (int i = 0; i <fullStateSize; ++i) {
			countSortSet[i] = 0;
		}

		int width = layer2.leftStates.nStates;

		for (int i = 0; i < layer1->initialStates.nStates; ++i) {
			int l1InitState = layer1->initialStates.set[i];
			for (int j = 0; j < layer2.leftStates.nStates; ++j) {
				countSortSet[l1InitState*width+j]+=1;
			}
		}

		for (int j = 0; j < layer2.initialStates.nStates; ++j) {
			int l2InitState = layer2.initialStates.set[j];
			for (int i = 0; i < layer1->leftStates.nStates; ++i) {
					countSortSet[i*width+l2InitState]+=1;
			}
		}
		int nStates = 0;
		for (int i = 0; i < layer1->leftStates.nStates; ++i) {
			for (int j = 0; j < layer2.leftStates.nStates; ++j) {
				if(countSortSet[i*width+j] >0){
					nStates +=1;
				}
			}
		}

		State* set = malloc(sizeof(State)*nStates);

		nStates = 0;
		for (int i = 0; i < layer1->leftStates.nStates; ++i) {
			for (int j = 0; j < layer2.leftStates.nStates; ++j) {
				if(countSortSet[i*width+j] >0){
					set[nStates] = i*width+j;
					nStates +=1;
				}
			}
		}

		result->initialStates.nStates = nStates;
		result->initialStates.set = set;

    }

	{
		int fullStateSize = layer1->rightStates.nStates*layer2.rightStates.nStates;
		int* countSortSet = malloc(sizeof(int)*fullStateSize);
		for (int i = 0; i <fullStateSize; ++i) {
			countSortSet[i] = 0;
		}

		int width = layer2.rightStates.nStates;

		for (int i = 0; i < layer1->finalStates.nStates; ++i) {
			int l1InitState = layer1->finalStates.set[i];
			for (int j = 0; j < layer2.rightStates.nStates; ++j) {
				countSortSet[l1InitState*width+j]+=1;
			}
		}

		for (int j = 0; j < layer2.finalStates.nStates; ++j) {
			int l2InitState = layer2.finalStates.set[j];
			for (int i = 0; i < layer1->rightStates.nStates; ++i) {
				countSortSet[i*width+l2InitState]+=1;
			}
		}
		//nStated can probably be computed in one line in constant time.
		int nStates = 0;
		for (int i = 0; i < layer1->rightStates.nStates; ++i) {
			for (int j = 0; j < layer2.rightStates.nStates; ++j) {
				if(countSortSet[i*width+j] > 0){
					nStates +=1;
				}
			}
		}
		result->finalStates.nStates = nStates;

		State* set = malloc(sizeof(State)*nStates);
		nStates = 0;
		for (int i = 0; i < layer1->rightStates.nStates; ++i) {
			for (int j = 0; j < layer2.rightStates.nStates; ++j) {
				if(countSortSet[i*width+j] >0){
					set[nStates] = i*width+j;
					nStates +=1;
				}
			}
		}
		result->finalStates.set = set;

	}

	int nTransitions = layer1->transitions.nTransitions*layer2.transitions.nTransitions;
	Transition* set = malloc(sizeof(Transition)*nTransitions);
	for(int i = 0; i< layer1->transitions.nTransitions; ++i){
		Transition l1Trance = layer1->transitions.set[i];
		for(int j = 0; j< layer2.transitions.nTransitions; ++j){
			Transition l2Trace = layer2.transitions.set[j];
			State s1 = l1Trance.s1*layer2.leftStates.nStates+l2Trace.s1;
			NumSymbol  a = l1Trance.a*layer2.map.sizeAlphabet+l2Trace.a;
			State s2 = l1Trance.s2*layer2.rightStates.nStates+l2Trace.s2;
			Transition t = {s1,a,s2};
			set[i*layer2.transitions.nTransitions+j]= t;
		}
	}

	result->transitions.set = set;
	result->transitions.nTransitions = nTransitions;

}
//Asuming the first odd is the return odd... Opposite of the tensor call ?
void tensorODD(ODD* odd1, ODD* odd2, ODD odd){
	int nLayers = odd2->nLayers;
	Layer* layerSequence = malloc(sizeof(Layer)*nLayers);
	int width = 0;
	for (int i = 0; i < nLayers; ++i) {
		tensorLayers(&odd2->layerSequence[i],odd.layerSequence[i], &layerSequence[i]);
		width = layerSequence[i].width > width ? layerSequence[i].width : width;
	}
	odd1->nLayers=nLayers;
	odd1->layerSequence = layerSequence;
	odd1->width = width;
}

