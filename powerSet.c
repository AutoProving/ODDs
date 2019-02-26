// Copyright 2019 Amund Lindberg and Markus Ragnhildstveit
// This file is licensed under MIT License, as specified in the file LICENSE located at the root folder of this repository.

#include <stdlib.h>
#include <math.h>
#include "odd.h"

int orderSet(int* S, Layer* layer) // maps subsets of integers to numbers. S has layer.width positions. S[i]=1 indicates that i belongs to S
{
    int sum = 0;
    for(size_t i = 0; i < layer->width; i++)
    {
        sum += S[i] * pow(2, i);
    }
    
    return sum;
}

int* next(int* S, NumSymbol a, Layer* layer) // Returns the set obtained by reading symbol a from some state in set S.  
{

}

void powerSetLayer(Layer* layer, Layer* result)
{

}

void powerSetODD(ODD* odd, ODD* result)
{

}