#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "odd.h"
#include "debugTools.h"
#include "cloneKill.h"

int main() {

    char filename[] = "ODD-Example.txt";
//    char filename[] = "ODD-Memo-Mapping.txt";
    ODD *odd = malloc(sizeof(ODD));
    readODD(filename, odd);

    ODD *clone = cloneODD(odd);
//    Layer *inputLayer = &odd->layerSequence[3];
//    Layer *resultLayer = malloc(sizeof(Layer));

    showODD(clone);

    return 0;
}