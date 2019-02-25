// Copyright 2019 YOUR NAME
// This file is licensed under MIT License, as specified in the file LISENSE located at the root folder of this repository.

#include <stdlib.h>
#include "odd.h"

NumSymbol readSymbol(char* filename){
    FILE *reader = fopen(filename, 'r');
    NumSymbol symbol;
    fscanf(reader, "%d", &symbol);
    fclose(reader);
    return symbol;
} // Reads a string representing the name of a symbol


void readAlphabetMap(char* filename, int nSymbols, AlphabetMap* map){
    map->sizeAlphabet = nSymbols;
    FILE *reader = fopen(filename, 'r');
    int number;
    char *string;
    for (int i = 0; i < nSymbols; i++){
        fscanf(reader, "%d %s", &number, string);
        map->N2S[i] = string;
        map->S2N[i] = number;
    }
    fclose(reader);
} // Reads nSymbols


State readState(char* filename){
    FILE *reader = fopen(filename, 'r');
    State state;
    fscanf(reader, "%d", &state);
    fclose(reader);
    return state;
} // Reads a state (represented by an int)


void readStates(char* filename, int nStates, StateContainer* states){
    states->nStates = nStates;
    for (int i = 0; i < nStates; i++){
        states->set[i] = readState(filename);
    }
}  // Reads nStates


Transition readTransition(char* filename){
    Transition t;
    t.s1 = readState(filename);
    t.a = readSymbol(filename);
    t.s2 = readState(filename);
    return t;

} // reads a transition represented by a triple of integers


void readTransitions(char* filename, int nTransitions, TransitionContainer* transitions){
    transitions->nTransitions = nTransitions;
    for(int i = 0; i < nTransitions; i++){
        transitions->set[i] = readTransition(filename);
    }
} // Reads nTransitions transitions


Layer readLayer(char* filename){
    AlphabetMap map;
    StateContainer leftStates, initialStates, rightStates, finalStates;
    TransitionContainer transitions;

    readAlphabetMap(filename, 5, &map);
    readStates(filename, 5, &leftStates);
    readStates(filename, 5, &rightStates);
    readStates(filename, 5, &initialStates);
    readStates(filename, 5, &finalStates);
    readTransitions(filename, 5, &transitions);

    Layer l = {.map = map,
               .leftStates = leftStates,
               .initialStates = initialStates,
               .rightStates = rightStates,
               .finalStates = finalStates,
               .width = leftStates.nStates > rightStates.nStates ? leftStates.nStates : rightStates.nStates,
               .initialFlag = 0,
               .finalFlag = 0};

    return l;
} // Reads a layer


void readODD(char* filename, ODD* odd){
    odd->nLayers = 5;
    odd->width = 5;
    for(int i = 0; i < odd->nLayers; i++){
        odd->layerSequence[i] = readLayer(filename);
    }
} // Reads a sequence of layers