#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "odd.h"

void testbitshift() {
    int layerwidth = 2;
    int lwpow = 4;
    int *s = calloc(layerwidth, sizeof(int));
    
    for (int i = 0; i < lwpow; i++) {

        printf("content of S: ");
        for(int j = 0; j < layerwidth; j++) {
            printf("%d ", s[j]);
        }
        printf("\n");
        

        // Do the bit shifting
        for(size_t i = 0; i < layerwidth; i++) {
            if (s[i] != 1) {
                s[i] = 1;
                break;
            } else {
                s[i] = 0;
            }
        }

    }
}

int main(){
    // printf() displays the string inside quotation
    printf("Hello, World!");

    ODD odd;
    readODD("ODD-Example.txt", &odd);

    printODD("test.txt", &odd);
    return 0;
}
