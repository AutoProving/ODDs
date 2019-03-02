#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "odd.h"
#include "debugTools.c"
#include "reading.c"
#include "memorization.c"


int main(){

    char filename[] = "ODD-Example.txt";
    printf("%zu", sizeof(ODD));
    ODD *odd = malloc(sizeof(ODD));;
    readODD(filename, odd);
    showODD(odd);

    Layer *oddLayer = &odd->layerSequence[0];
    Layer newLayer;

    showLayer(&odd->layerSequence[0]);
    showLayer(&odd->layerSequence[1]);
    showLayer(&odd->layerSequence[2]);
    showLayer(&odd->layerSequence[3]);

//    oddLayer->width = 666;
//
//    showODD(odd);

    return 0;
}