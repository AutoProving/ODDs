#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "odd.h"
#include "debugTools.c"
#include "reading.c"
#include "memorization.c"

int main() {

//    char filename[] = "ODD-Example.txt";
    char filename[] = "ODD-Memo-Mapping.txt";
    ODD *odd = malloc(sizeof(ODD));
    ODD *oddResult = malloc(sizeof(ODD));
    readODD(filename, odd);

//    Layer *inputLayer = &odd->layerSequence[3];
//    Layer *resultLayer = malloc(sizeof(Layer));


    printf("----BEFORE MEMORIZE-----\n\n");
    showODD(odd);
//    showLayer(inputLayer);

    memorizeODD(odd, oddResult);
//    memorizeLayer(inputLayer, resultLayer);

    printf("----AFTER MEMORIZE-----\n\n");
    showODD(oddResult);
//    showLayer(resultLayer);
    return 0;
}