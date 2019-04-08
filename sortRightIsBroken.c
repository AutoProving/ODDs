// Copyright 2019  Konstantin Rygol
// This file is licensed under MIT License, as specified in the file LISENSE located at 
//the root folder of this repository.
#include <stdio.h>
#include <stdlib.h>
#include "odd.h"
#include <omp.h> //also add -fopenmp to compile path
#include "debugTools.h" 
int main(int argc,char **argv){
    ODD odd;
    readODD("ODD-ToMinimize.txt",&odd); 
    int numLayers = odd.nLayers;
    printf("Number of Layers: %d \n", numLayers);
    printODD("ODD-Mysave_before.txt",&odd);
    //sortLeftTransitions(&(odd.layerSequence[numLayers-1].transitions));
    sortAllRightTransitions(&odd);
    printODD("ODD-Mysave_after.txt",&odd);
    fprintf(stderr,"sort right transitions was called\n");
    return 0; 
}


