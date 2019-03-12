// Copyright 2019 Mateus de Oliveira Oliveira
// This file is licensed under MIT License, as specified in the file LISENSE located at the root folder of this repository.

#ifndef ODD_H
#define ODD_H

#include <stdio.h>
#include <stdbool.h>

///////////////////////////// CONVENTIONS /////////////////////////////////////////////////////
//
// New type names start with Captal letters.
// New variable and function names start with lower-case letters.
// Compound names are named LikeThis for types and likeThis for variables.
//
// index variables: i, i1, i2, ...
// state: s, s1, s2 ...
// set of states: states, states1, states2, ...
// set of transitions: transitions, transitions1, transitions2 ...
// symbol: a, a1, a2
// transition: t, t1, t2 ...
// layer: l, l1, l2, ...
// odd: odd
//
//
//
///////////////////////////////////////////////////////////////////////////////////



/////////////////////////////// NumSymbol type ////////////////////////////////////
typedef int NumSymbol;
////////////////////////////////////////////////////////////////////////////////




////////////////////////////// State type /////////////////////////////////////////
typedef int State;
///////////////////////////////////////////////////////////////////////////////////


/////////////////////////// Transition type ///////////////////////////////////////
typedef struct{
	State s1;
	NumSymbol a;
	State s2;
} Transition;

//////////////////////////////////////////////////////////////////////////////////



/////////////////////// StateContainer type //////////////////////////////////////
typedef struct{
	int nStates;
	State* set; // Should be sorted
} StateContainer;


// Use function findState(states,s) if you want to determine if a state s belongs to states
State* findState(StateContainer* states, State* s); // returns address of state t or NULL if t is not there

void sortStates(StateContainer* states); // Sorts states.set lexicographically


/////////////////////////////////////////////////////////////////////////////////


////////////////// TransitionstateContainer type ////////////////////////////////
typedef struct{
	int nTransitions;
	Transition* set; // Should be sorted
} TransitionContainer;


// Use function findTransition(transitions,t) if you want to determine if a transition t belongs to transitions
Transition* findTransition(TransitionContainer* transitions, Transition* t); // returns address of transition t or NULL if t is not there

///////////////////////////////////////////////////////////////////////////////



///////////////////////////// AlphabetMap type //////////////////////////////////

typedef struct{
	int sizeAlphabet; // Number of symbols in the alphabet
	char** N2S; // Each Number in the range [0...sizeAlphabet-1] is converted into a String.
	// Strings may have distinct sizes and therefore space for them must be allocated individually.
	int* S2N; // This is an integer array where N2S[S2N[i]]<N2S[S2N[i+1]] for each i. In other words, the numbers
	// in the array are ordered lexicographically with respect to their respective strings. Given a string
	// S we can use this map together with binary search to find the unique number j such that MapN2S[j]=S.
	// This is useful when dealing with large alphabets.
} AlphabetMap;

char* symbolName(NumSymbol a); // returns N2S[a]
int numberSymbol(char* name); // returns the number of the the symbol named name.



//////////////////////////////////////////////////////////////////////////////////////



// Defines Layer type
typedef struct{
	AlphabetMap map; // Obs: The map can vary from layer to layer
	StateContainer leftStates;
	StateContainer initialStates;
	StateContainer rightStates;
	StateContainer finalStates;
	TransitionContainer transitions;
	int width; // equal to max{leftStates.nStates, rightStates.nStates}
	bool initialFlag; // set to 1 if if this is the first layer of the ODD
	bool finalFlag;   // set to 1 if this is the last layer of the ODD
} Layer;



// Defines ODD type
typedef struct{
	int nLayers; // Length of ODD
	int width;   // The maximum width of a layer
	Layer* layerSequence;
} ODD;


//Sorting - functions (Functions implemented in file sorting.c)
void sortAllLeftTransitions(ODD* odd);
void sortAllRightTransitions(ODD* odd);
void sortLeftTransitions(TransitionContainer* transitions); // Sorts transitions.set lexicographically
void sortRightTransitions(TransitionContainer* transitions); // Sorts transitions.set lexicographically
void mergesort(TransitionContainer* transitions, int lo, int hi, bool sortLeft);
void merge(TransitionContainer* transitions, int leftLo, int leftHi, int rightLo, int rightHi, bool sortLeft);


//Read from file. (Functions implemented in file reading.c)
void readSymbol(FILE* reader, NumSymbol *symbol); // Reads a string representing the name of a symbol
void readAlphabetMap(FILE* reader, AlphabetMap* map); // Reads nSymbols
void readState(FILE* reader, State *state); // Reads a state (represented by an int)
void readStates(FILE* reader, StateContainer* states);  // Reads nStates
void readTransition(FILE* reader, Transition *transition); // reads a transition represented by a triple of integers
void readTransitions(FILE* reader, TransitionContainer* transitions); // Reads nTransitions transitions
void readLayer(FILE* reader, Layer* layer); // Reads a layer
void readODD(char* filename, ODD* odd); // Reads a sequence of layers

//Printing to file. (Functions implemented in file printing.c)

void printSymbol(FILE* filename, NumSymbol s);
void printAlphabetMap(FILE* filename, AlphabetMap* map);
void printState(FILE* filename, State s);
void printStates(FILE* filename, StateContainer* states);
void printTransition(FILE* filename, Transition t);
void printTransitions(FILE* filename, TransitionContainer* transitions);
void printLayer(FILE* filename, Layer* layer);
void printODD(char* filename, ODD* odd);


//Membership Testing (Numerical and String)  (Functions implemented in file membership.c)

bool numMembership(ODD* odd, int* numSeq); // returns 1 iff the sequence numSeq is accepted by odd
bool strMembership(ODD* odd, char** strSeq); // returns 1 if the sequence of symbol names represented by strSeq is accepted by odd


//Union (Functions implemented in file union.c)

void unionLayers(Layer* layer1, Layer* layer2, Layer* result);
void uniontODDs(ODD* odd1, ODD* odd2, ODD* odd);

//Intersection (Functions implemented in file intersection.c)


void intersectionLayers(Layer* layer1, Layer* layer2, Layer* result);
void intersectionODD(ODD* odd1, ODD* odd2, ODD* odd);

//tensorProduct (Functions implemented in file tensorProduct.c)


void tensorLayers(Layer* layer1, Layer* layer2, Layer* result);
void tensorODD(ODD* odd1, ODD* odd2, ODD* odd);

//test deterministic (Functions implemented in file deterministic.c)


bool isDeterministicLayer(Layer* layer);
bool isDeterministicODD(ODD* odd);

//test complete (Functions implemented in file complete.c)


bool isCompleteLayer(Layer* layer);
bool isCompleteODD(ODD* odd);


//Negation (Functions implemented in file negation.c)


void negationLayer(Layer* layer, Layer* result);
void negationODD(ODD* odd, ODD* result);


//Power Set (Functions implemented in file powerSet.c)


int orderSet(int* S, Layer* layer); // maps subsets of integers to numbers. S has layer.width positions. S[i]=1 indicates that i belongs to S
int* next(int* S, NumSymbol a, Layer* layer); // Returns the set obtained by reading symbol a from some state in set S.
void powerSetLayer (Layer* layer, Layer* result);
void powerSetODD(ODD* odd, ODD* result);

//Completion (Functions implemented in file completion.c)


void makeCompleteLayer(Layer* layer, Layer* result);
void makeCompleteODD(ODD* odd, ODD* result);


//Memorization (Functions implemented in file memorization.c)


void memorizeLayer(Layer* layer, Layer* result);
void memorizeODD(ODD* odd, ODD* result);

//minimization (Functions implemented in file minimization.c)


void minimizeODD(ODD* odd, ODD* result);

#endif
