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

//    ODD *clone = cloneODD(odd);
//    Layer *inputLayer = &odd->layerSequence[3];
//    Layer *resultLayer = malloc(sizeof(Layer));


    printf("\n\nBEFORE MEMO\n\n");
    showODD(odd);
//    showLayer(&odd->layerSequence[1]);
//    odd->layerSequence[1] = *memorizeLayer(&odd->layerSequence[1]);
    ODD* memorized = memorizeODD(odd);
    printf("\n\nAFTER MEMO\n\n");
//    showLayer(&odd->layerSequence[1]);
    showODD(memorized);

//    printf("\n\nDOUBLE CHECK ORIGINAL\n\n");
//    showODD(odd);

//    showODD(clone);

//    printf("*odd: %p\n"
//           "*clone: %p\n"
//           "Equal: %s\n", odd, clone, (odd == clone) ? "YES" : "NO");
    return 0;
}