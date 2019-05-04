// Copyright 2019 Markus Ragnhildstveit, Amund Lindberg, Ole Magnus Morken, Guillermo Hoyo Bravo and Josep Barbera Muñoz
// This file is licensed under MIT License, as specified in the file LISENSE located at the root folder of this repository.

#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <omp.h>
#include "odd.h"

typedef struct StateList
{
    State st;
    struct StateList *next;
} StateList;

typedef struct LinkedList
{
    struct StateList *states;
    int size;
    struct LinkedList *next;
} LinkedList;

//TODO: comments
//A_sz is the number of right states in a layer

Layer *lazy_power(Layer *l, LinkedList *map, int w, int h, LinkedList **right_map, int *A_sz);
LinkedList *create_next_list(Layer *l, LinkedList *map, int w, int h, int *A_sz);
void create_initial_list(ODD *odd, LinkedList *last);
LinkedList *nextLazy(LinkedList *S, NumSymbol a, Layer *layer);
void freeStateList(StateList *lst, int sz);
void freeLinkedList(LinkedList *lst, int sz);

//Returns the right state if found in A, -1 if not.
State number(LinkedList *next, LinkedList *A, Layer *l, int A_sz);

//Returns -1 if s1 is lexicographically smaller than s2, 0 if equal, 1 if larger
int compare(LinkedList *s1, LinkedList *s2);

ODD *lazy_power_ODD(ODD *odd)
{
    ODD *result = (ODD *)malloc(sizeof(ODD));

    result->nLayers = odd->nLayers;
    result->layerSequence = calloc(odd->nLayers, sizeof(Layer));

    // Creating the initial left_map
    LinkedList *left_map = (LinkedList *)malloc(sizeof(LinkedList));
    create_initial_list(odd, left_map);

    LinkedList *right_map = NULL;
    int h = 1;

    for (int i = 0; i < odd->nLayers; i++)
    {
        int A_sz = 0;
        result->layerSequence[i] = *lazy_power(&odd->layerSequence[i], left_map, odd->width, h, &right_map, &A_sz);
        freeLinkedList(left_map, h);
        left_map = right_map;
        h = A_sz;
    }
    freeLinkedList(right_map, h);

    // The width of the resulting lazy-power-ODD must be equal to its widest layer.
    result->width = 0;
    for (int i = 0; i < result->nLayers; i++)
    {
        if (result->layerSequence[i].width > result->width)
        {
            result->width = result->layerSequence[i].width;
        }
    }

    return result;
}

void create_initial_list(ODD *odd, LinkedList *left_map)
{

    left_map->states = malloc(sizeof(StateList));
    StateList *temp = left_map->states;
    left_map->size = 0;

    for (int i = 0; i < odd->layerSequence[0].initialStates.nStates; i++)
    {
        if (i > 0)
        {
            temp->next = malloc(sizeof(StateList));
            temp = temp->next;
        }

        temp->st = odd->layerSequence[0].initialStates.set[i];
        left_map->size++;
    }
    left_map->next = NULL;
}

Layer *lazy_power(Layer *l, LinkedList *map, int w, int h, LinkedList **right_map, int *A_sz)
{
    int asz = 0;
    LinkedList *A = create_next_list(l, map, w, h, &asz);
    Layer *result = malloc(sizeof(Layer));
    result->leftStates.nStates = h;
    result->rightStates.nStates = asz;
    result->map = l->map;
    result->initialFlag = l->initialFlag;
    result->finalFlag = l->finalFlag;

    if (result->initialFlag)
    {
        result->initialStates.nStates = h;
        result->initialStates.set = malloc(h * sizeof(int));
    }
    else
    {
        result->initialStates.nStates = 0;
    }

    result->finalStates.nStates = 0;
    if (result->finalFlag)
    {
        result->finalStates.set = malloc(asz * sizeof(State));
        LinkedList *temp = A;
        for (int i = 0; i < asz; i++)
        {
            bool inserted = false;
            StateList *tempState = temp->states;
            for (int j = 0; j < temp->size && !inserted; j++)
            {
                for (int fi = 0; fi < l->finalStates.nStates && !inserted; fi++)
                {
                    if (tempState->st == l->finalStates.set[fi])
                    {
                        result->finalStates.set[result->finalStates.nStates] = i;
                        result->finalStates.nStates++;
                        inserted = true;
                    }
                }
                tempState = tempState->next;
            }
            temp = temp->next;
        }
    }

    result->leftStates.set = malloc(h * sizeof(int));
    for (int i = 0; i < h; i++)
    {
        result->leftStates.set[i] = i;
        if (result->initialFlag)
        {
            result->initialStates.set[i] = i;
        }
    }

    result->rightStates.set = malloc(asz * sizeof(int));
    for (int j = 0; j < asz; j++)
    {
        result->rightStates.set[j] = j;
    }

    result->transitions.nTransitions = 0;
    result->transitions.set = malloc(h * l->map.sizeAlphabet * (sizeof(Transition)));
    LinkedList *subset = map;
    for (int k = 0; k < h && subset != NULL; k++)
    {
        for (int ix = 0; ix < l->map.sizeAlphabet; ix++)
        {
            State s2 = number(nextLazy(subset, l->map.S2N[ix], l), A, l, asz);
            if (s2 != -1)
            {
                result->transitions.set[result->transitions.nTransitions].s1 = k;
                result->transitions.set[result->transitions.nTransitions].a = l->map.S2N[ix];
                result->transitions.set[result->transitions.nTransitions].s2 = s2;
                result->transitions.nTransitions++;
            }
        }
        subset = subset->next;
    }

    result->width = fmax(result->leftStates.nStates, result->rightStates.nStates);
    *right_map = A;
    *A_sz = asz;
    return result;
}

LinkedList *create_next_list(Layer *l, LinkedList *map, int w, int h, int *A_sz)
{
    LinkedList *A;
    int asz = 0;
    LinkedList *subset = map;

    while (h > 0 && subset != NULL)
    {
        for (int alphi = 0; alphi < l->map.sizeAlphabet; alphi++)
        {
            LinkedList *S = nextLazy(subset, l->map.S2N[alphi], l);
            S->next = NULL;

            if (S->size > 0 && asz == 0)
            {
                A = S;
                asz++;
            }
            else if (S->size > 0)
            {
                LinkedList *it = A;
                // Find correct position and add it to the linkedlist
                int compared = compare(it, S);
                if (compared == -1)
                {
                    A = S;
                    A->next = it;
                    asz++;
                }
                else if (compared != 0)
                {
                    compared = compare(it->next, S);
                    while (it != NULL && compared != 0)
                    {
                        if (it->next == NULL || compared == -1)
                        {
                            S->next = it->next;
                            it->next = S;
                            asz++;
                            it = NULL;
                        }
                        else
                        {
                            it = it->next;
                            if (it != NULL)
                            {
                                compared = compare(it->next, S);
                            }
                        }
                    }
                }
                else
                {
                    freeLinkedList(S, 1);
                }
            }
        }
        subset = subset->next;
        h--;
    }

    *A_sz = asz;
    return A;
}

State number(LinkedList *next, LinkedList *A, Layer *l, int A_sz)
{
    if (next->size == 0)
        return -1;

    LinkedList *B = A;
    for (int i = 0; i < A_sz && B != NULL; i++)
    {
        if (compare(next, B) == 0)
        {
            freeLinkedList(next, 1);
            return i;
        }

        B = B->next;
    }

    freeLinkedList(next, 1);
    return -1;
}

LinkedList *nextLazy(LinkedList *S, NumSymbol a, Layer *layer)
{
    LinkedList *result = malloc(sizeof(LinkedList));
    result->states = malloc(sizeof(StateList));
    result->size = 0;
    StateList *sout = result->states;
    StateList *temp = S->states;

    for (int i = 0; i < S->size; i++)
    {
        State s1 = temp->st;

        for (int j = 0; j < layer->transitions.nTransitions; j++)
        {
            if (layer->transitions.set[j].s1 == s1 && layer->transitions.set[j].a == a)
            {
                // Insert in order without duplicates
                StateList *souttemp = sout;
                bool inserted = false;
                while (souttemp != NULL && !inserted)
                {
                    if (result->size == 0)
                    {
                        souttemp->st = layer->transitions.set[j].s2;
                        result->size++;
                        souttemp->next = NULL;
                    }
                    else
                    {
                        if (souttemp->st == layer->transitions.set[j].s2)
                        {
                            inserted = true;
                        }
                        else if (souttemp == sout && souttemp->st > layer->transitions.set[j].s2)
                        {
                            StateList *statelistTemp = malloc(sizeof(StateList));
                            statelistTemp->st = layer->transitions.set[j].s2;
                            statelistTemp->next = sout;
                            result->states = statelistTemp;
                            sout = statelistTemp;
                            result->size++;
                            inserted = true;
                        }
                        else if (souttemp->next == NULL || souttemp->next->st > layer->transitions.set[j].s2)
                        {
                            StateList *statelistTemp = malloc(sizeof(StateList));
                            statelistTemp->st = layer->transitions.set[j].s2;
                            statelistTemp->next = souttemp->next;
                            souttemp->next = statelistTemp;
                            result->size++;
                            inserted = true;
                        }
                    }

                    souttemp = souttemp->next;
                }
            }
        }
        temp = temp->next;
    }

    return result;
}

int compare(LinkedList *s1, LinkedList *s2)
{
    if (s1 == NULL || s2 == NULL)
        return -1;

    StateList *t1 = s1->states;
    StateList *t2 = s2->states;
    for (int i = 0; i < fmin(s1->size, s2->size); i++)
    {
        if (t1->st > t2->st)
            return -1;
        if (t1->st < t2->st)
            return 1;

        t1 = t1->next;
        t2 = t2->next;
    }

    if (s1->size > s2->size)
        return -1;
    if (s1->size < s2->size)
        return 1;

    return 0;
}

void freeLinkedList(LinkedList *lst, int sz)
{
    for (int i = 0; i < sz; i++)
    {
        LinkedList *next = lst->next;
        freeStateList(lst->states, lst->size);
        free(lst);
        lst = next;
    }
}

void freeStateList(StateList *lst, int sz)
{
    for (int i = 0; i < sz && lst != NULL; i++)
    {
        StateList *next = lst->next;
        free(lst);
        lst = next;
    }
}