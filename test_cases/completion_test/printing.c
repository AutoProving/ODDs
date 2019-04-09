// Copyright 2019 YOUR NAME
// This file is licensed under MIT License, as specified in the file LISENSE located at the root folder of this repository.

#include "odd.h"
#include <stdio.h>
#include <stdlib.h>

/**
	Will print a number n and its corresponding string N2S[n].
*/
/*
void printSymbol(FILE *f, NumSymbol s)
{
	
}
*/

/**
	Calls printSymbol for each element in the map.
*/
void printAlphabetMap(FILE *f, AlphabetMap* map)
{
	for(int i = 0; i < map->sizeAlphabet; i++) 
	{
		fprintf(f, "%d %s\n", i, map->N2S[i]);
	}
}

/**
	Prints a single state to file: filename.
*/
void printState(FILE *f, State s)
{
	fprintf(f, "%d\n", s);
}

/**
	Calls printState for each state.
*/
void printStates(FILE *f, StateContainer* states)
{
	for(int i = 0; i < states->nStates; i++)
	{
		printState(f, states->set[i]);
	} 
}

/**
	Prints a single transition to file: filename.
*/
void printTransition(FILE *f, Transition t)
{
	fprintf(f, "%d %d %d\n", t.s1, t.a, t.s2);
} 

/**
	Calls printTransition for each transition.
*/
void printTransitions(FILE *f, TransitionContainer* transitions)
{
	for(int i = 0; i < transitions->nTransitions; i++) 
	{
		printTransition(f, transitions->set[i]); 
	}
}

/**
	Will call printAlphabetMap, printStates, and printTransitions.
*/
void printLayer(FILE *f, Layer* layer)
{
	fprintf(f, "LAYER\nALPHABET_MAP %d\n", layer->map.sizeAlphabet);
	printAlphabetMap(f, &(layer->map));
	fprintf(f, "LEFT_STATES %d\n", layer->leftStates.nStates);
	printStates(f, &(layer->leftStates));
	fprintf(f, "RIGHT_STATES %d\n", layer->rightStates.nStates);
	printStates(f, &(layer->rightStates));
	fprintf(f, "INITIAL_STATES %d\n", layer->initialStates.nStates);
	printStates(f, &(layer->initialStates));
	fprintf(f, "FINAL_STATES %d\n", layer->finalStates.nStates);
	printStates(f, &(layer->finalStates));
	fprintf(f, "TRANSITIONS %d\n", layer->transitions.nTransitions);
	printTransitions(f, &(layer->transitions));
	fprintf(f, "INITIAL_FLAG %d\n", layer->initialFlag);
	fprintf(f, "FINAL_FLAG %d\n", layer->finalFlag);
}

/**
	Will call printlayer a number of times.
*/
void printODD(char* filename, ODD* odd)
{
	FILE* f = fopen(filename, "w");
	if (f == NULL)
	{
		printf("Error opening the file!\n");
		exit(1);
	}
	fprintf(f, "ODD %d\n", odd->nLayers);
	for(int i = 0; i < odd->nLayers; i++)
	{
		printLayer(f, &(odd->layerSequence[i]));
		fprintf(f, "\n");
	}
	fclose(f);
}
