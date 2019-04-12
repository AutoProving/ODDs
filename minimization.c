// Copyright 2019 Christian Stafset, Daniel Lansdaal, Konstantin Rygol
// This file is licensed under MIT License, as specified in the file LISENSE located at the root folder of this repository.
#include <stdio.h>
#include <stdlib.h>
#include "odd.h"
#include "debugTools.h"
#include <omp.h> //also add -fopenmp to compile path
#include <string.h>

void minimize(ODD* o, ODD* result);

int findIndexState(StateContainer* v, State s) {
    //returns index i such that v[i]=s
    //binary search
    int bottom = 0;
    int top = v->nStates-1;
    int index = (bottom + top) / 2;
    int guess = v->set[index];
    while (guess != s) {
        if (s < guess) {
            top = index-1;
        }
        else {
            bottom = index+1;
        }
        index = (bottom + top) / 2;
        guess = v->set[index];
    }
    return index;
}

void cleanLeft(Layer* l);
void cleanRight(Layer* l);
void cleanTransitions(Layer* l);
void cleanTransitionsRight(Layer* l);
void copyStates(StateContainer* l1, StateContainer* l2);

int main(int argc, char** argv) {
    ODD o;
    ODD res;

    readODD("ODD-ToMinimize2.txt", &o);
    showODD(&o);
    minimize(&o, &res);
    showODD(&o);

}

void copyStates(StateContainer* l1, StateContainer* l2) {
    l2->nStates = l1->nStates;
    l2->set = memcpy(l2->set, l1->set, l1->nStates*sizeof(State));
    l2->set = realloc(l2->set, l2->nStates*sizeof(State));
}

int findIndexTransition(TransitionContainer T, Transition t) {
    //returns index i such that T[i] = t
}

int countUsefulStates(StateContainer s) {
    int n = 0;
    for (int i = 0; i < s.nStates; i++) {
        if (s.set[i] >= 0) {
            n++;
        }
    }
    return n;
}

void cleanTransitionsRight(Layer* l) {

	StateContainer* leftStates = &(l->leftStates);
	StateContainer* rightStates = &(l->rightStates);
	TransitionContainer* transitions = &(l->transitions);

	TransitionContainer auxTransitions;
	auxTransitions.set = malloc(transitions->nTransitions * sizeof(Transition));
	auxTransitions.nTransitions = transitions->nTransitions;
	for (int i = transitions->nTransitions - 1; i >= 0; i--) {
		auxTransitions.set[i].s2 = -1;
	}

	int i = 0;
	int curTransition = 0;
	int usefulTransitions = 0;

	for (int i = 0; i < rightStates->nStates; i--) {

		while (transitions->set[curTransition].s2 <= rightStates->set[i] && curTransition < transitions->nTransitions) {
			if (transitions->set[curTransition].s2 == rightStates->set[i]) {
				transitions->set[usefulTransitions] = transitions->set[curTransition];
				usefulTransitions++;
			}
			curTransition++;
		}
	}

	transitions->nTransitions = usefulTransitions;
	transitions->set = realloc(transitions->set, usefulTransitions * sizeof(Transition));

 //	for (int i = numLayers - 1; i >= 0; i--)
}

void cleanTransitions(Layer* l) {

    StateContainer* leftStates = &(l->leftStates);
    StateContainer* rightStates = &(l->rightStates);
    TransitionContainer* transitions = &(l->transitions);

    TransitionContainer auxTransitions;
    auxTransitions.set = malloc(transitions->nTransitions * sizeof(Transition));
    auxTransitions.nTransitions = transitions->nTransitions;
    for (int i = 0; i < transitions->nTransitions; i++) {
        auxTransitions.set[i].s1 = -1;
    }

    int i = 0;
    int curTransition = 0;
    int usefulTransitions = 0;

    for (int i = 0; i < leftStates->nStates; i++) {
        while (transitions->set[curTransition].s1 <= leftStates->set[i] && curTransition < transitions->nTransitions) {
            if (transitions->set[curTransition].s1 == leftStates->set[i]) {
                transitions->set[usefulTransitions] = transitions->set[curTransition];
                usefulTransitions++;
            }
            curTransition++;
        }
    }

    transitions->nTransitions = usefulTransitions;
    transitions->set = realloc(transitions->set, usefulTransitions * sizeof(Transition));

}

void cleanLeft(Layer* l) {

	StateContainer* usefulRight = &(l->rightStates);
	StateContainer* usefulLeft = &(l->leftStates);

	//fprintf(stderr, "accomplished, %i\n", usefulLeft->nStates);

	StateContainer auxStates;
	auxStates.set = malloc(usefulLeft->nStates * sizeof(State));
	auxStates.nStates = usefulLeft->nStates;
	for (int i = 0; i < usefulLeft->nStates; i++) {
		auxStates.set[i] = -1;
	}

	int transitionIndex = 0;
	int i = 0;
	//for each useful right state
	while (i < usefulRight->nStates) {
		while (l->transitions.set[transitionIndex].s2 <= usefulRight->set[i] && transitionIndex < l->transitions.nTransitions) {
			if (l->transitions.set[transitionIndex].s2 == usefulRight->set[i]) {
				// If useful, change value in aux state from -1
				State* s = &l->transitions.set[transitionIndex].s1;
				int index = findIndexState(usefulLeft, *s);
				auxStates.set[findIndexState(usefulLeft, *s)] = *s;
			}
			// check next transition
			transitionIndex++;
		}
		// go to next state
		i++;
	}
	int n = countUsefulStates(auxStates);

	usefulLeft->set = realloc(usefulLeft->set, n * sizeof(State));
	usefulLeft->nStates = n;
	int index = 0;

	for (int i = 0; i < auxStates.nStates; i++) {
		if (auxStates.set[i] != -1) {
			usefulLeft->set[index] = auxStates.set[i];
			index++;
		}
	}
}

void cleanRight(Layer* l) {

    StateContainer* usefulLeft = &(l->leftStates);
    StateContainer* usefulRight = &(l->rightStates);

    //fprintf(stderr, "accomplished, %i\n", usefulRight->nStates);
    
    StateContainer auxStates;
    auxStates.set = malloc(usefulRight->nStates * sizeof(State));
    auxStates.nStates = usefulRight->nStates;
    for (int i = 0; i < usefulRight->nStates; i++) {
        auxStates.set[i] = -1;
    }

    int transitionIndex = 0;
    int i = 0;
    //for each useful left state
    while (i < usefulLeft->nStates) {
        //for each transition with left state as current left state (or there is a dead state with no transition coming out which is smaller)
        while (l->transitions.set[transitionIndex].s1 <= usefulLeft->set[i] && transitionIndex < l->transitions.nTransitions) {
            //fprintf(stderr, "State Index: %i\nTransitionIndex: %i\nUseful Transitions: %i\n\n", i, transitionIndex);

            //if the state transition has current useful state as left state
            if (l->transitions.set[transitionIndex].s1 == usefulLeft->set[i]) {
                //set it as useful by changing aux state to not -1
                State* s = &l->transitions.set[transitionIndex].s2;
                int index = findIndexState(usefulRight, *s);
                auxStates.set[findIndexState(usefulRight, *s)] = *s;
            }
            //next transition
            transitionIndex++;
        }
        //next state
        i++;
    }

    int n = countUsefulStates(auxStates);

    usefulRight->set = realloc(usefulRight->set, n*sizeof(State));
    usefulRight->nStates = n;
    int index = 0;

    for (int i = 0; i < auxStates.nStates; i++) {
        if (auxStates.set[i] != -1) {
            usefulRight->set[index] = auxStates.set[i];
            index++;
        }
    }
}

void minimize(ODD* o, ODD* result) {

    int numLayers = o->nLayers;

    copyStates(&(o->layerSequence[0].initialStates), &(o->layerSequence[0].leftStates));

        fprintf(stderr, "BEGIN:\n\n");
    for (int i = 0; i < numLayers; i++) {
        cleanTransitions(&(o->layerSequence[i]));
        cleanRight(&(o->layerSequence[i]));
        if (i < numLayers - 1) {
            copyStates(&(o->layerSequence[i].rightStates), &(o->layerSequence[i+1].leftStates));
        }
        
    }
	void sortAllRightTransitions(ODD* o);
    	fprintf(stderr, "BEGIN RtL:\n\n");
        sortAllRightTransitions(o);
	for (int i = numLayers - 1; i >= 0; i--){
		cleanLeft(&(o->layerSequence[i]));
		cleanTransitionsRight(&(o->layerSequence[i]));

		if (i > 0) {
			copyStates(&(o->layerSequence[i].leftStates), &(o->layerSequence[i-1].rightStates));
		}
	}
    sortAllLeftTransitions(o);
}
