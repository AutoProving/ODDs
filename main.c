#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "odd.h"
#include "debugTools.c"
#include "reading.c"
#include "memorization.c"

int main(){

    char filename[] = "ODD-Example.txt";
    ODD *odd = malloc(sizeof(ODD));
    readODD(filename, odd);
//    showODD(odd);

    Layer *inputLayer = &odd->layerSequence[0];
    Layer *resultLayer = malloc(sizeof(Layer));

    printf("\ninputLayer BEFORE memorizeLayer(inputLayer, resultLayer);\n\n");
    showLayer(inputLayer);
    memorizeLayer(inputLayer, resultLayer);

    printf("\nresultLayer AFTER memorizeLayer(inputLayer, resultLayer);\n\n");
    showLayer(resultLayer);


    printf("\n\n-----Show ODD-----\n\n");
    showODD(odd);

    return 0;
}