#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../../odd.h"
#include "../../debugTools.h"
#include "../../cloneKill.h"

void localMemo(ODD *odd, int i);

int main(){

    printf("--------------------{Testing memorization.c}--------------------\n");

    char fileName[] = "../../ODD-Example.txt";
    ODD *inputODD = malloc(sizeof(ODD));
    readODD(fileName, inputODD);

    printf("%s\n", "#####################");
    printf("%s\n", "##### Input ODD #####");
    printf("%s\n", "#####################");
    showODD(inputODD);

    printf("%s\n", "#########################");
    printf("%s\n", "##### memorizeODD() #####");
    printf("%s\n", "#########################");
    ODD *memorizedODD = memorizeODD(inputODD);
    showODD(memorizedODD);

    printf("%s\n", "#######################");
    printf("%s\n", "##### localMemo() #####");
    printf("%s\n", "#######################");
    for (int i = 0; i < inputODD->nLayers-1; ++i)
        localMemo(inputODD,i);
    showODD(inputODD);

    killODD(inputODD);
    killODD(memorizedODD);

    return 0;
}