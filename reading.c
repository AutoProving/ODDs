// Copyright 2019 Ole Magnus Lie
// This file is licensed under MIT License, as specified in the file LISENSE located at the root folder of this repository.

#include <stdlib.h>
#include <string.h>
#include "odd.h"

void skip(FILE *reader)
{
    if (fscanf(reader, "%*[^\n]\n") == EOF)
    {
        fprintf(stderr, "Unexpected end of file.\n");
        exit(EXIT_FAILURE);
    }
}

void readAlphabetMap(FILE *reader, AlphabetMap *map)
{

    while (fscanf(reader, "ALPHABET_MAP %d", &map->sizeAlphabet) != 1)
    {
        skip(reader);
    }

    map->S2N = malloc(map->sizeAlphabet * sizeof(int));
    map->N2S = malloc(map->sizeAlphabet * sizeof(char *));

    for (int i = 0; i < map->sizeAlphabet; i++)
    {
        map->N2S[i] = malloc(sizeof(char *));
        while (fscanf(reader, "%d %s", &map->S2N[i], map->N2S[i]) != 2)
        {
            skip(reader);
        }
    }
} // Reads nSymbols

void readStates(FILE *reader, char *keyword, StateContainer *states)
{
    while (fscanf(reader, keyword, &states->nStates) != 1)
    {
        skip(reader);
    }

    states->set = malloc(states->nStates * sizeof(State *));

    for (int i = 0; i < states->nStates; i++)
    {
        while (fscanf(reader, "%d\n", &states->set[i]) != 1)
        {
            skip(reader);
        }
    }
} // Reads nStates

void readTransitions(FILE *reader, TransitionContainer *transitions)
{
    while (fscanf(reader, "TRANSITIONS %d", &transitions->nTransitions) != 1)
    {
        skip(reader);
    }

    transitions->set = malloc(transitions->nTransitions * sizeof(Transition));

    for (int i = 0; i < transitions->nTransitions; i++)
    {
        while (
            fscanf(reader, "%d %d %d\n", &transitions->set[i].s1, &transitions->set[i].a, &transitions->set[i].s2) != 3)
        {
            skip(reader);
        }
    }
} // Reads nTransitions transitions

void readLayer(FILE *reader, Layer *layer)
{
    while (fscanf(reader, "LAYER") != 0)
    {
        skip(reader);
    }

    readAlphabetMap(reader, &layer->map);
    readStates(reader, "LEFT_STATES %d", &layer->leftStates);
    readStates(reader, "RIGHT_STATES %d", &layer->rightStates);
    readStates(reader, "INITIAL_STATES %d", &layer->initialStates);
    readStates(reader, "FINAL_STATES %d", &layer->finalStates);
    readTransitions(reader, &layer->transitions);

    layer->width = layer->leftStates.nStates > layer->rightStates.nStates ? layer->leftStates.nStates : layer->rightStates.nStates;
    int initalTemp, finalTemp;
    while (fscanf(reader, "INITIAL_FLAG %d", &initalTemp) != 1)
    {
        skip(reader);
    }

    while (fscanf(reader, "FINAL_FLAG %d", &finalTemp) != 1)
    {
        skip(reader);
    }

    layer->initialFlag = initalTemp;
    layer->finalFlag = finalTemp;

} // Reads a layer

void readODD(char *filename, ODD *odd)
{
    FILE *reader = fopen(filename, "r");

    if (reader != NULL)
    {
        while (fscanf(reader, "ODD %d", &odd->nLayers) != 1)
        {
            skip(reader);
        }

        odd->width = 0;
        odd->layerSequence = malloc(odd->nLayers * sizeof(Layer));

        for (int i = 0; i < odd->nLayers; i++)
        {
            readLayer(reader, &odd->layerSequence[i]);
            odd->width = odd->layerSequence[i].width > odd->width ? odd->layerSequence[i].width : odd->width;
        }
        fclose(reader);
    }
    else
    {
        fprintf(stderr, "Error opening file.\n");
    }

} // Reads a sequence of layers
