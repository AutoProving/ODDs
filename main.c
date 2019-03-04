#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "odd.h"
#include "debugTools.c"
#include "reading.c"
#include "memorization.c"

int main() {

    char filename[] = "ODD-Example.txt";
    ODD *odd = malloc(sizeof(ODD));
    ODD *oddResult = malloc(sizeof(ODD));
    readODD(filename, odd);

    printf("----BEFORE MEMORIZE-----\n\n");
    showODD(odd);

    memorizeODD(odd, oddResult);

    printf("----AFTER MEMORIZE-----\n\n");
    showODD(oddResult);

    return 0;
}