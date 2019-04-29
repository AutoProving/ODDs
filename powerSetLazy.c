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

Layer *lazy_power(Layer *l, LinkedList *map, int w, int h, LinkedList **right_map, int *asz);
LinkedList *create_next_list(Layer *l, LinkedList *map, int w, int h, int *A_sz);
void createInitialList(ODD *odd, LinkedList *last);
int compareTo(LinkedList *s1, LinkedList *s2);
LinkedList *next(LinkedList *S, NumSymbol a, Layer *layer);
State number(LinkedList *next, LinkedList *A, Layer *l, int a_sz);

ODD *lazy_power_ODD(ODD *odd)
{
    ODD *result = (ODD *)malloc(sizeof(ODD));
    int oddwidth = odd->width;
    LinkedList *right_map = NULL;

    // Creating the initial left_map
    LinkedList *left_map = (LinkedList *)malloc(sizeof(LinkedList));
    LinkedList *last = left_map;

    createInitialList(odd, last);

    int h = 1;
    result->layerSequence = calloc(odd->nLayers, sizeof(Layer));
    result->nLayers = odd->nLayers;
    for (int i = 0; i < odd->nLayers; i++)
    {
        int asz = 0;
        result->layerSequence[i] = *lazy_power(&odd->layerSequence[i], left_map, oddwidth, h, &right_map, &asz);
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

void createInitialList(ODD *odd, LinkedList *last)
{
    int oddwidth = odd->width;

    last->states = malloc(sizeof(StateList));
    StateList *temp = last->states;
    int sz = 0;

    for (int i = 0; i < odd->layerSequence[0].initialStates.nStates; i++)
    {
        if (i != 0)
        {
            temp->next = malloc(sizeof(StateList));
            temp = temp->next;
        }

        temp->st = odd->layerSequence[0].initialStates.set[i];
        sz++;
    }
    last->size = sz;
    last->next = NULL;
}

Layer *lazy_power(Layer *l, LinkedList *map, int w, int h, LinkedList **right_map, int *asz)
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
        result->initialStates.nStates = h;
        result->initialStates.set = malloc(h * sizeof(int));
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
        if (result->initialFlag)
        {
            result->initialStates.set[i] = i;
        }
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
            State s2 = number(next(subset, l->map.S2N[ix], l), A, l, A_sz);
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
    *asz = A_sz;
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
            LinkedList *S = next(subset, l->map.S2N[alphi], l);
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
                int compared = compareTo(it, S);
                if (compared == -1)
                {
                    A = S;
                    A->next = it;
                    asz++;
                }
                else if (compared != 0)
                {
                    int compared = compareTo(it->next, S);
                    while (it != NULL && compared != 0)
                    {
                        if (it->next == NULL || compared == -1)
                        {
                            S->next = it->next;
                            it->next = S;
                            it->next->next = NULL;
                            asz++;
                            it = NULL;
                        }
                        else
                        {

                            it = it->next;
                        }

                        if (it != NULL)
                        {
                            compared = compareTo(it->next, S);
                        }
                    }
                }
            }
        }
        subset = subset->next;
        h--;
    }

    *A_sz = asz;
    return A;
}

State number(LinkedList *next, LinkedList *A, Layer *l, int a_sz)
{
    LinkedList *B = A;
    if (next->size == 0)
    {
        return -1;
    }

    for (int i = 0; i < a_sz && B != NULL; i++)
    {
        if (compareTo(next, B) == 0)
        {
            return i;
        }

        B = B->next;
    }

    return -1;
}

// int orderSet(LinkedList *S, Layer *layer)
// {
//     return orderSetWidth(S->data, S->size);
// }

// int orderSetWidth(int *S, int width)
// {
//     int sum = 0;
//     for (int i = 0; i < width; i++)
//     {
//         sum += pow(2, S[i]);
//     }

//     return sum - 1;
// }

LinkedList *next(LinkedList *S, NumSymbol a, Layer *layer)
{
    LinkedList *linkedList = malloc(sizeof(LinkedList));
    linkedList->states = malloc(sizeof(StateList));
    StateList *sout = linkedList->states;
    int sz = 0;
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
                    if (sz == 0)
                    {
                        souttemp->st = layer->transitions.set[j].s2;
                        sz++;
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
                            linkedList->states = statelistTemp;
                            sout = statelistTemp;
                            sz++;
                            inserted = true;
                        }
                        else if (souttemp->next == NULL || souttemp->next->st > layer->transitions.set[j].s2)
                        {
                            StateList *statelistTemp = malloc(sizeof(StateList));
                            statelistTemp->st = layer->transitions.set[j].s2;
                            statelistTemp->next = souttemp->next;
                            souttemp->next = statelistTemp;
                            sz++;
                            inserted = true;
                        }
                    }

                    souttemp = souttemp->next;
                }
            }
        }
        temp = temp->next;
    }
    linkedList->size = sz;

    return linkedList;
}

// bool isEqual(LinkedList *s1, LinkedList *s2)
// {
//     if (s1->size != s2->size)
//     {
//         return false;
//     }
//     StateList *t1 = s1->states;
//     StateList *t2 = s2->states;
//     for (int i = 0; i < s1->size; i++)
//     {
//         if (t1->st != t2->st)
//         {
//             return false;
//         }
//         t1 = t1->next;
//         t2 = t2->next;
//     }
//     return true;
// }

int compareTo(LinkedList *s1, LinkedList *s2)
{
    if (s2 == NULL || s1 == NULL)
    {
        return -1;
    }

    StateList *t1 = s1->states;
    StateList *t2 = s2->states;
    for (int i = 0; i < fmin(s1->size, s2->size); i++)
    {
        if (t1->st > t2->st)
        {
            return -1;
        }
        else if (t1->st < t2->st)
        {
            return 1;
        }

        t1 = t1->next;
        t2 = t2->next;
    }

    if (s1->size > s2->size)
    {
        return -1;
    }
    else if (s1->size < s2->size)
    {
        return 1;
    }

    return 0;
}