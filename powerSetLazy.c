#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include "odd.h"
#include "powerSet.c"

typedef struct LinkedList
{
    int *data;
    int order;
    struct LinkedList *next;
} LinkedList;

Layer *lazy_power(Layer *l, LinkedList *map, int w, int h, LinkedList *right_map, int *asz);
LinkedList *create_next_list(Layer *l, LinkedList *map, int w, int h, int *A_sz);
void createList(ODD* odd, LinkedList* last);
State number(int *next, LinkedList *A, Layer *l, int a_sz);

ODD *lazy_power_ODD(ODD *odd)
{
    ODD *result = (ODD *)malloc(sizeof(ODD));
    int oddwidth = odd->width;
    struct LinkedList *right_map = NULL;

    // Creating the initial left_map
    struct LinkedList *left_map = (struct LinkedList *)malloc(sizeof(struct LinkedList));
    struct LinkedList *last = left_map;
    
    createList(odd, last);

    int h = pow(2, odd->layerSequence[0].initialStates.nStates) - 1;
    result->layerSequence = malloc(odd->nLayers);
    result->nLayers = odd->nLayers;
    for (int i = 0; i < odd->nLayers; i++)
    {
        int asz = 0;
        result->layerSequence[i] = *lazy_power(&odd->layerSequence[i], left_map, oddwidth, h, right_map, &asz);
        left_map = right_map;
        h = asz;
    }

    // Find the max width
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

void createList(ODD* odd, LinkedList* last) 
{
    int oddwidth = odd->width;

    int *sinit = calloc(odd->layerSequence[0].initialStates.nStates, sizeof(int));
    sinit[0] = 1;
    for (int i = 1; i < pow(2, odd->layerSequence[0].initialStates.nStates); i++)
    {
        if (i != 1)
        {
            struct LinkedList *l = (struct LinkedList *)malloc(sizeof(struct LinkedList));
            last->next = l;
            last = l;
        }

        last->data = calloc(oddwidth, sizeof(int));
        for(int j = 0; j < odd->layerSequence[0].initialStates.nStates; j++)
        {
            if (sinit[j]) {
                last->data[odd->layerSequence[0].initialStates.set[j]] = 1;
            }
        }
        
        last->order = orderSetWidth(last->data, oddwidth);
        last->next = NULL;
        bitshift(sinit, odd->layerSequence[0].initialStates.nStates);
    }
}

Layer *lazy_power(Layer *l, LinkedList *map, int w, int h, LinkedList *right_map, int *asz)
{
    int A_sz = 0;
    LinkedList *A = create_next_list(l, map, w, h, &A_sz);
    Layer *result = malloc(sizeof(Layer));
    result->leftStates.nStates = h;
    result->rightStates.nStates = A_sz;
    result->map = l->map;
    result->initialFlag = l->initialFlag;
    result->finalFlag = l->finalFlag;

    // TODO
    if (result->initialFlag)
    {
        result->initialStates.nStates = l->initialStates.nStates;
        result->initialStates.set = l->initialStates.set;
    }
    else
    {
        result->initialStates.nStates = 0;
    }

    // TODO
    if (result->finalFlag)
    {
        result->finalStates.nStates = l->finalStates.nStates;
        result->finalStates.set = l->finalStates.set;
    }
    else
    {
        result->finalStates.nStates = 0;
    }

    result->leftStates.set = malloc(h * sizeof(int));
    for (int i = 0; i < h; i++)
    {
        result->leftStates.set[i] = i;
    }

    result->rightStates.set = malloc(A_sz * sizeof(int));
    for (int j = 0; j < A_sz; j++)
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
            result->transitions.set[result->transitions.nTransitions].s1 = k;
            result->transitions.set[result->transitions.nTransitions].a = l->map.S2N[ix];
            result->transitions.set[result->transitions.nTransitions].s2 = number(next(subset->data, l->map.S2N[ix], l), A, l, A_sz);
            result->transitions.nTransitions++;
        }
        subset = subset->next;
    }

    result->width = fmax(result->leftStates.nStates, result->rightStates.nStates);
    right_map = A;
    *asz = A_sz;
    return result;
}

LinkedList *create_next_list(Layer *l, LinkedList *map, int w, int h, int *A_sz)
{
    struct LinkedList *A = (struct LinkedList *)malloc(sizeof(struct LinkedList));
    struct LinkedList *last = A;
    int asz = 0;
    bool isEmpty = true;
    struct LinkedList *subset = map;
    while (subset != NULL)
    {
        for (int alphi = 0; alphi < l->map.sizeAlphabet; alphi++)
        {
            int *S = next(subset->data, l->map.S2N[alphi], l);
            bool isInA = false;
            int ordS = orderSet(S, l);
            struct LinkedList *it = A;
            if (isEmpty)
            {
                A->data = S;
                A->order = ordS;
                A->next = NULL;
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
        subset = subset->next;
    }

    *A_sz = asz;
    return A;
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