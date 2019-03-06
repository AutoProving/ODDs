#include <stdio.h>
#include <stdbool.h>
#include "odd.h"


int main(){
    ODD odd;
    // printf() displays the string inside quotation
    readODD("ODD-Example.txt", &odd);
    Layer result;
     FILE *fil = fopen("output.txt", "w");
     FILE *fil2=fopen("output1.txt","w");
     FILE *fil3=fopen("output2.txt","w");

    unionLayers(&(odd.layerSequence[0]), &(odd.layerSequence[1]), &result);

    printLayer(fil,&result);
    printLayer(fil2,&(odd.layerSequence[0]));
    printLayer(fil3, &(odd.layerSequence[1]));
    return 0;
}