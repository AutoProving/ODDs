// Copyright 2019 Ole Magnus Lie
// This file is licensed under MIT License, as specified in the file LICENSE located at the root folder of this repository.

#include <stdlib.h>
#include <string.h>
#include "odd.h"

void readSymbol(FILE *reader, NumSymbol *symbol){
    fscanf(reader, "%d", symbol);
} // Reads a string representing the name of a symbol


void readAlphabetMap(FILE *reader, AlphabetMap* map){
    char line[64], c[32];
    while(fgets(line, sizeof(line), reader))
        if (strstr(line, "ALPHABET_MAP") != NULL)
            break;

    sscanf(line, "%s %d", c, &map->sizeAlphabet);
    map->S2N = malloc(map->sizeAlphabet * sizeof(int));
    map->N2S = malloc(map->sizeAlphabet * sizeof(char*));

    for (int i = 0; i < map->sizeAlphabet; i++){
        fscanf(reader, "%d %s", map->S2N, *map->N2S);
    }
} // Reads nSymbols


void readState(FILE *reader, State *state){
    fscanf(reader, "%d", state);
} // Reads a state (represented by an int)


void readStates(FILE *reader, StateContainer* states){
    char line[64], c[32];
    while(fgets(line, sizeof(line), reader))
        if (strstr(line, "STATES") != NULL)
            break;

    sscanf(line, "%s %d", c, &states->nStates);
    states->set = malloc(states->nStates * sizeof(State*));
    for (int i = 0; i < states->nStates; i++){
        State state;
        readState(reader, &state);
        states->set[i] = state;
    }
}  // Reads nStates


void readTransition(FILE *reader, Transition *transition){
    readState(reader, &transition->s1);
    readSymbol(reader, &transition->a);
    readState(reader, &transition->s2);
} // reads a transition represented by a triple of integers


void readTransitions(FILE *reader, TransitionContainer* transitions){
    char line[64], c[32];
    while(fgets(line, sizeof(line), reader))
        if (strstr(line, "TRANSITIONS") != NULL)
            break;

    sscanf(line, "%s %d", c, &transitions->nTransitions);
    transitions->set = malloc(transitions->nTransitions * sizeof(Transition));
    for(int i = 0; i < transitions->nTransitions; i++){
        Transition transition;
        readTransition(reader, &transition);
        transitions->set[i] = transition;
    }
} // Reads nTransitions transitions


void readLayer(FILE *reader, Layer* layer){
    AlphabetMap map;
    StateContainer leftStates, initialStates, rightStates, finalStates;
    TransitionContainer transitions;

    char line[64], c[32];
    while(fgets(line, sizeof(line), reader))
        if (strstr(line, "LAYER") != NULL)
            break;

    readAlphabetMap(reader, &map);
    readStates(reader, &leftStates);
    readStates(reader, &rightStates);
    readStates(reader, &initialStates);
    readStates(reader, &finalStates);
    readTransitions(reader, &transitions);

    layer->map = map;
    layer->leftStates = leftStates;
    layer->rightStates = rightStates;
    layer->initialStates = initialStates;
    layer->finalStates = finalStates;
    layer->width = leftStates.nStates > rightStates.nStates ? leftStates.nStates : rightStates.nStates;

    while(fgets(line, sizeof(line), reader)) {
        int flag;
        if (strstr(line, "INITIAL_FLAG") != NULL) {
            sscanf(line, "%s %d", c, &flag);
            layer->initialFlag = flag == 1;
        }
        if (strstr(line, "FINAL_FLAG") != NULL) {
            sscanf(line, "%s %d", c, &flag);
            layer->finalFlag = flag == 1;
        }
    }
} // Reads a layer


void readODD(char* filename, ODD* odd){
    FILE *reader = fopen(filename, "r");
    char line[64], c[32];

    while(fgets(line, sizeof(line), reader))
        if (strstr(line, "ODD") != NULL)
            break;

    sscanf(line, "%s %d", c, &odd->nLayers);

    odd->width = 0;
    odd->layerSequence = malloc(odd->nLayers * sizeof(Layer));

    for(int i = 0; i < odd->nLayers; i++){
        Layer layer;
        readLayer(reader, &layer);

        odd->layerSequence[i] = layer;
        odd->width = odd->layerSequence[i].width > odd->width ? odd->layerSequence[i].width : odd->width;
    }
    fclose(reader);
} // Reads a sequence of layers
