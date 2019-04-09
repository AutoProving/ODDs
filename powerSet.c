// Copyright 2019 Amund Lindberg and Markus Ragnhildstveit
// This file is licensed under MIT License, as specified in the file LICENSE located at the root folder of this repository.

#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include "odd.h"

typedef struct LinkedList
{
    int *data;
    int order;
    struct LinkedList *next;
} LinkedList;

// Some helper functions
void setStateSize(StateContainer *oldstate, StateContainer *powerstate);
void bitshift(int *S, int sz);
int findStateIndex(Layer *layer, State s2, int j);
int hasCommonStates(int *S, StateContainer rightstate, StateContainer finstate);
int isSubSet(int *subS, int *S, int sz);
void showTransitions(TransitionContainer transitions);
void showStates(StateContainer states);
void testPowerSetODD(ODD odd, ODD powerODD);
void intToBitArray(int num, int *S, int sz);
LinkedList *create_next_list(Layer *l, LinkedList *map, int w, int h, int *A_sz);
State number(int *next, LinkedList *A, Layer *l, int a_sz);
Layer *lazy_power(Layer *l, LinkedList *map, int w, int h, LinkedList *right_map, int *asz);
// End helper functions

ODD *powerSetODD(ODD *odd)
{
    ODD *result = malloc(sizeof(ODD));
    result->nLayers = odd->nLayers;
    result->layerSequence = malloc(odd->nLayers * sizeof(Layer));
    result->width = 0;
#pragma omp parallel for
    for (int i = 0; i < odd->nLayers; i++)
    {
        result->layerSequence[i] = *powerSetLayer(&odd->layerSequence[i]);
    }

    // Find max width of the layers
    for (int i = 0; i < odd->nLayers; i++)
    {
        if (result->layerSequence[i].width > result->width)
        {
            result->width = result->layerSequence[i].width;
        }
    }

    return result;
}

Layer *powerSetLayer(Layer *layer)
{
    Layer *result = malloc(sizeof(Layer));
    //SET FLAGS
    result->initialFlag = layer->initialFlag;
    result->finalFlag = layer->finalFlag;

    //SET ALPHABET MAP
    result->map.sizeAlphabet = layer->map.sizeAlphabet;
    result->map.N2S = layer->map.N2S;
    result->map.S2N = layer->map.S2N;

    //ALLOCATE FOR POWER-STATES
    setStateSize(&layer->leftStates, &result->leftStates);
    setStateSize(&layer->rightStates, &result->rightStates);

    if (layer->initialStates.nStates > 0)
    {
        setStateSize(&layer->initialStates, &result->initialStates);
    }
    result->initialStates.nStates = 0;

    if (layer->finalStates.nStates > 0)
    {
        setStateSize(&layer->finalStates, &result->finalStates);
    }
    result->finalStates.nStates = 0;

    int maxsz = pow(2, layer->width);

    //ALLOCATE FOR POWER-TRANSITIONS
    result->transitions.nTransitions = 0;
    result->transitions.set = malloc(maxsz * layer->map.sizeAlphabet * sizeof(Transition));

    int *initialS = calloc(layer->width, sizeof(int));
    for (int i = 0; i < layer->initialStates.nStates; i++)
    {
        int posNotFound = 1;
        for (int j = 0; posNotFound && j < layer->leftStates.nStates; j++)
        {
            if (layer->initialStates.set[i] == layer->leftStates.set[j])
            {
                initialS[j] = 1;
                posNotFound = 0;
            }
        }
    }

    int *S = calloc(layer->width, sizeof(int));
    for (int i = 0; i < maxsz; i++)
    {
        int order = orderSet(S, layer);

        if (i < result->leftStates.nStates)
        {
            // Left states:
            result->leftStates.set[i] = order;

            // Initial states:
            if (layer->initialStates.nStates > 0 && i > 0 && isSubSet(S, initialS, layer->width))
            {
                result->initialStates.set[result->initialStates.nStates] = order;
                result->initialStates.nStates++;
            }

            // Transitions:
            for (int j = 0; j < layer->map.sizeAlphabet; j++)
            {

                int a = layer->map.S2N[j];
                int ordernext = orderSet(next(S, a, layer), layer);

                if (ordernext != 0)
                {
                    result->transitions.set[result->transitions.nTransitions].s1 = order;
                    result->transitions.set[result->transitions.nTransitions].a = a;
                    result->transitions.set[result->transitions.nTransitions].s2 = ordernext;
                    result->transitions.nTransitions++;
                }
            }
        }

        if (i < result->rightStates.nStates)
        {
            // Right states:
            result->rightStates.set[i] = order;

            // Final states:
            if (layer->finalStates.nStates > 0 && hasCommonStates(S, layer->rightStates, layer->finalStates))
            {
                result->finalStates.set[result->finalStates.nStates] = order;
                result->finalStates.nStates++;
            }
        }
        bitshift(S, layer->width);
    }
    free(S);
    free(initialS);
    result->width = fmax(result->leftStates.nStates, result->rightStates.nStates);

    return result;
}

// maps subsets of integers to numbers. S has layer.width positions. S[i]=1 indicates that i belongs to S
int orderSet(int *S, Layer *layer)
{
    int sum = 0;
    for (int i = 0; i < layer->width; i++)
    {
        sum += S[i] * pow(2, i);
    }

    return sum;
}

// Returns the set obtained by reading symbol a from some state in set S.
int *next(int *S, NumSymbol a, Layer *layer)
{
    int *sout = calloc(layer->width, sizeof(int));

    for (int i = 0; i < layer->width; i++)
    {
        if (S[i])
        {
            for (int j = 0; j < layer->transitions.nTransitions; j++)
            {
                if (layer->transitions.set[j].s1 == layer->leftStates.set[i] && layer->transitions.set[j].a == a)
                {
                    sout[findStateIndex(layer, layer->transitions.set[j].s2, j)] = 1;
                }
            }
        }
    }
    return sout;
}

State number(int *next, LinkedList *A, Layer *l, int a_sz)
{
    int next_order = orderSet(next, l);
    LinkedList *B = A;
    for (int i = 0; i < a_sz && B != NULL; i++)
    {
        if (next_order == B->order)
        {
            return i;
        }

        B = B->next;
    }

    return -1;
}

ODD *lazy_power_ODD(ODD *odd)
{
    ODD *res = (ODD*)malloc(sizeof(ODD));
    struct LinkedList *right_map = NULL;

    // Creating the initial left_map
    struct LinkedList *left_map = (struct LinkedList *)malloc(sizeof(struct LinkedList));
    struct LinkedList *last = left_map;
    for (int i = 0; i < odd->layerSequence[0].initialStates.nStates; i++)
    {
        if (i != 0) {
            struct LinkedList *l = (struct LinkedList *)malloc(sizeof(struct LinkedList));
            last->next = l;
            last = l;
        }
        last->data = calloc(odd->width, sizeof(int));
        last->data[findStateIndex(&odd->layerSequence[0], odd->layerSequence[0].initialStates.set[i], i)] = 1;
        last->order = orderSet(last->data, &odd->layerSequence[0]);
        last->next = NULL;
        
    }

    int h = odd->layerSequence[0].initialStates.nStates;
    res->layerSequence = malloc(odd->nLayers);
    res->nLayers = odd->nLayers;
    for (int i = 0; i < odd->nLayers; i++)
    {
        int asz = 0;
        res->layerSequence[i] = *lazy_power(&odd->layerSequence[i], left_map, odd->width, h, right_map, &asz);
        left_map = right_map;
        h = asz;
    }
    
    // Find the max width
    res->width = 0;
    for (int i = 0; i < res->nLayers; i++)
    {
        if (res->layerSequence[i].width > res->width)
        {
            res->width = res->layerSequence[i].width;
        }
    }

    return res;
}

Layer *lazy_power(Layer *l, LinkedList *map, int w, int h, LinkedList *right_map, int *asz)
{
    int A_sz = 0;
    LinkedList *A = create_next_list(l, map, w, h, &A_sz);
    Layer *L = malloc(sizeof(Layer));
    L->leftStates.nStates = h;
    L->rightStates.nStates = A_sz;
    L->map = l->map;
    L->initialFlag = l->initialFlag;
    L->finalFlag = l->finalFlag;

    //TODO
    if (L->initialFlag) {
        L->initialStates.nStates = l->initialStates.nStates;
        L->initialStates.set = l->initialStates.set;
    } 
    else
    {
        L->initialStates.nStates = 0;
    }

    // TODO
    if (L->finalFlag) {
        L->finalStates.nStates = l->finalStates.nStates;
        L->finalStates.set = l->finalStates.set;
    }
    else
    {
        L->finalStates.nStates = 0;
    }
    
    
    

    L->leftStates.set = malloc(h * sizeof(int));
    for (int i = 0; i < h; i++)
    {
        L->leftStates.set[i] = i;
    }

    L->rightStates.set = malloc(A_sz * sizeof(int));
    for (int j = 0; j < A_sz; j++)
    {
        L->rightStates.set[j] = j;
    }

    L->transitions.nTransitions = 0;
    L->transitions.set = malloc(h * l->map.sizeAlphabet * (sizeof(Transition)));
    LinkedList *temp = map;
    for (int k = 0; k < h && temp != NULL; k++)
    {
        for (int ix = 0; ix < l->map.sizeAlphabet; ix++)
        {
            L->transitions.set[L->transitions.nTransitions].s1 = k;
            L->transitions.set[L->transitions.nTransitions].a = l->map.S2N[ix];
            L->transitions.set[L->transitions.nTransitions].s2 = number(next(temp->data, l->map.S2N[ix], l), A, l, A_sz);
            L->transitions.nTransitions++;
        }
        temp = temp->next;
    }

    L->width = fmax(L->leftStates.nStates, L->rightStates.nStates);
    right_map = A;
    *asz = A_sz;
    return L;
}

LinkedList *create_next_list(Layer *l, LinkedList *map, int w, int h, int *A_sz)
{
    struct LinkedList *A = (struct LinkedList *)malloc(sizeof(struct LinkedList));
    struct LinkedList *last = A;
    A->next = NULL;
    int asz = 0;
    bool isEmpty = true;
    struct LinkedList *temp = map;
    for (int i = 0; i < h && temp != NULL; i++)
    {
        for (int alphi = 0; alphi < l->map.sizeAlphabet; alphi++)
        {
            int *S = next(temp->data, l->map.S2N[alphi], l);
            bool isInA = false;
            int ordS = orderSet(S, l);
            struct LinkedList *it = A;
            if (isEmpty)
            {
                A->data = S;
                A->order = ordS;
                asz++;
                isEmpty = false;
            }
            else
            {
                it = A;
                // Find correct position and add it to the linkedlist
                while (it != NULL && !isInA)
                {
                    if (ordS == it->order)
                    {
                        isInA = true;
                    }
                    else if (it->next == NULL || ordS < it->next->order)
                    {
                        struct LinkedList *l = (struct LinkedList *)malloc(sizeof(struct LinkedList));
                        last->next = l;
                        last = l;
                        last->data = S;
                        last->order = ordS;
                        last->next = NULL;
                        asz++;
                    }

                    it = it->next;
                }
            }
        }
        temp = temp->next;
    }

    *A_sz = asz;
    return A;
}

///////////////////////////////////////
//helper functions below

void intToBitArray(int num, int *S, int sz)
{
    unsigned int mask = 1U << (sz - 1);

    for (int i = 0; i < sz; i++)
    {
        S[i] = (num & mask) ? 1 : 0;
        num <<= 1;
    }
}

void setStateSize(StateContainer *oldstate, StateContainer *powerstate)
{
    int sz = pow(2, oldstate->nStates);
    powerstate->nStates = sz;
    // powerstate->set = calloc(sz, sizeof(State));
    powerstate->set = malloc(sz * sizeof(int));
}

void bitshift(int *S, int sz)
{
    for (size_t i = 0; i < sz; i++)
    {
        if (S[i] != 1)
        {
            S[i] = 1;
            return;
        }
        else
        {
            S[i] = 0;
        }
    }
}

int isSubSet(int *subS, int *S, int sz)
{
    for (int i = 0; i < sz; i++)
    {
        if (subS[i] == 1 && S[i] != 1)
        {
            return 0;
        }
    }

    return 1;
}

int hasCommonStates(int *S, StateContainer rightstate, StateContainer finstate)
{
    for (int i = 0; i < rightstate.nStates; i++)
    {
        if (S[i])
        {
            for (size_t j = 0; j < finstate.nStates; j++)
            {
                if (finstate.set[j] == rightstate.set[i])
                {
                    return 1;
                }
            }
        }
    }

    return 0;
}

//
int findStateIndex(Layer *layer, State s2, int j)
{
    if (layer->rightStates.set[s2] == s2)
    {
        return s2;
    }
    else
    {
        for (size_t i = 0; i < layer->rightStates.nStates; i++)
        {
            if (layer->rightStates.set[i] == s2)
            {
                return i;
            }
        }
    }

    perror("Could not find the state in next-function.");
    exit(-1);
}

void showTransitions(TransitionContainer transitions)
{
    printf("{");
    for (int i = 0; i < transitions.nTransitions; i++)
    {
        printf("(%d, %d, %d)", transitions.set[i].s1, transitions.set[i].s2, transitions.set[i].a);
        if (i != transitions.nTransitions - 1)
        {
            printf(", ");
        }
    }
    printf("}\n");
}

void showStates(StateContainer states)
{
    printf("{");
    for (int i = 0; i < states.nStates - 1; i++)
    {
        printf("%d, ", states.set[i]);
    }
    printf("%d}\n", states.set[states.nStates - 1]);
}

void testPowerSetODD(ODD odd, ODD powerODD)
{
    printf("\n------------------------ Result of PowerSetODD ---------------------------\n");
    printf("--------------------------------------------------------------------------\n");
    printf("Original ODD has width: %d and powerODD has width: %d\n", odd.width, powerODD.width);
    printf("Original ODD has %d layers and powerODD has %d layers\n", odd.nLayers, powerODD.nLayers);

    for (int i = 0; i < powerODD.nLayers; i++)
    {

        Layer pl = powerODD.layerSequence[i];
        printf("\nPowerLayer %d with initFlag = %d, finalFlag = %d\n", i, pl.initialFlag, pl.finalFlag);

        if (pl.initialStates.nStates > 0)
        {
            printf("InitialStates: ");
            showStates(pl.initialStates);
        }

        printf("LeftStates: ");
        showStates(pl.leftStates);

        printf("RightStates: ");
        showStates(pl.rightStates);

        if (pl.finalStates.nStates > 0)
        {
            printf("FinalStates: ");
            showStates(pl.finalStates);
        }

        printf("Transitions: ");
        showTransitions(pl.transitions);

        //alphabetmap
    }

    printf("--------------------------------------------------------------------------\n");
    printf("--------------------------------------------------------------------------\n");
    printf("Done printing powerODD\n");
}
