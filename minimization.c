// Copyright 2019 Christian Stafset, Daniel Lansdaal, Konstantin Rygol
// This file is licensed under MIT License, as specified in the file LISENSE located at the root folder of this repository.
#include <stdio.h>
#include <stdlib.h>
#include "odd.h"
#include "debugTools.h"
#include <omp.h> //also add -fopenmp to compile path


int findIndexState(StateContainer v, State s) {
    //returns index i such that v[i]=s
    //binary search
    int bottom = -1;
    int top = v.nStates;
    int index = (bottom + top) / 2;
    int guess = v.set[index];
    while (guess != s) {
        if (s < guess) {
            top = index;
        }
        else {
            bottom = index;
        }
        index = (bottom + top) / 2;
        guess = v.set[index];
    }
    return index;
}

void cleanRight(Layer* l, StateContainer usefulLeft, StateContainer usefulRight);

int main(int argc, char** argv) {
    Layer l;

    l.initialFlag = 1;
    l.initialStates.set = malloc(0 * sizeof(State));
    l.initialStates.nStates = 0;

    l.finalFlag = 0;
    l.finalStates.set = malloc(0 * sizeof(State));
    l.finalStates.nStates = 0;

    l.map.sizeAlphabet = 1;
    l.map.N2S = malloc(sizeof(char*));
    l.map.N2S[0] = "a";

    l.width = 5;

    l.leftStates.set = malloc(4 * sizeof(State));
    l.leftStates.nStates = 4;
    l.rightStates.set = malloc(4 * sizeof(State));
    l.rightStates.nStates = 4;
    l.transitions.set = malloc(7 * sizeof(Transition));
    l.transitions.nTransitions = 7;

    for (int i = 0; i < 4; i++) {
        l.leftStates.set[i] = i;
        l.rightStates.set[i] = i;
    }

    l.transitions.set[0].s1 = 0;
    l.transitions.set[0].a = 1;
    l.transitions.set[0].s2 = 0;

    l.transitions.set[1].s1 = 0;
    l.transitions.set[1].a = 1;
    l.transitions.set[1].s2 = 1;

    l.transitions.set[2].s1 = 0;
    l.transitions.set[2].a = 1;
    l.transitions.set[2].s2 = 3;

    l.transitions.set[3].s1 = 1;
    l.transitions.set[3].a = 1;
    l.transitions.set[3].s2 = 0;

    l.transitions.set[4].s1 = 1;
    l.transitions.set[4].a = 1;
    l.transitions.set[4].s2 = 3;

    l.transitions.set[5].s1 = 2;
    l.transitions.set[5].a = 1;
    l.transitions.set[5].s2 = 1;

    l.transitions.set[6].s1 = 4;
    l.transitions.set[6].a = 1;
    l.transitions.set[6].s2 = 2;

    showLayer(&l);

    cleanRight(&l, l.leftStates, l.rightStates);

    showLayer(&l);

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



void cleanRight(Layer* l, StateContainer usefulLeft, StateContainer usefulRight) {
    l->leftStates = usefulLeft;
    StateContainer auxStates;
    auxStates.set = malloc(usefulRight.nStates * sizeof(State));
    auxStates.nStates = usefulRight.nStates;
    for (int i = 0; i < usefulRight.nStates; i++) {
        auxStates.set[i] = -1;
    }
    int usefulTransitions = 0;
    int transitionIndex = 0;
    int i = 0;
    while (i < l->transitions.nTransitions) {
        while (l->transitions.set[transitionIndex].s1 <= usefulLeft.set[i]) {
            if (l->transitions.set[transitionIndex].s1 == usefulLeft.set[i]) {
                usefulTransitions++;
                State s = l->transitions.set[transitionIndex].s2;
                auxStates.set[findIndexState(usefulRight, s)] = s;
            }
            transitionIndex++;
        }
        i++;
    }

    int n = countUsefulStates(auxStates);

    StateContainer newRight;
    newRight.nStates = n;
    newRight.set = malloc(n * sizeof(State));

    TransitionContainer newTrans;
    newTrans.nTransitions = usefulTransitions;
    newTrans.set = malloc(usefulTransitions * sizeof(Transition));

    i = 0;
    int stateIndex = 0;
    transitionIndex = 0;
    int usefulTransIndex = 0;
    while (i < usefulLeft.nStates) {
        while (l->transitions.set[transitionIndex].s1 <= usefulLeft.set[i]) {
            if (l->transitions.set[transitionIndex].s1 == usefulLeft.set[i]) {
                newTrans.set[usefulTransIndex] = l->transitions.set[transitionIndex];
                usefulTransIndex++;
            }
            transitionIndex++;
        }
        i++;
    }

    for (int i = 0; i < l->rightStates.nStates; i++) {
        State s = auxStates.set[i];
        if (s >= 0) {
            newRight.set[stateIndex] = s;
            stateIndex++;
        }
    }

    l->rightStates = newRight;
    l->transitions = newTrans;

    l->width = (l->rightStates.nStates > l->leftStates.nStates) ? l->rightStates.nStates : l->leftStates.nStates;
}

void minimize(ODD* o, ODD* result) {

    int numLayers = o->nLayers;

    o->layerSequence[0].leftStates = o->layerSequence[0].initialStates;

    for (int i = 0; i < numLayers; i++) {
        cleanRight(&(o->layerSequence[i]), o->layerSequence[i].leftStates, o->layerSequence[i].rightStates);
    }

}