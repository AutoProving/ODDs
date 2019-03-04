#include <stdio.h>
#include <stdbool.h>
#include "odd.h"


int main(){
    // printf() displays the string inside quotation
    ODD odd;
    readODD("ODD-Example.txt", &odd);
    printODD("test.txt", &odd);

    isDeterministicODD(&odd);
    return 0;
}
