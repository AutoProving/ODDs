#include <stdio.h>
#include <stdbool.h>
#include "odd.h"


void printStringArrayStatus(ODD* od3, char** stringArray){
    for (int i = 0; i < 5; ++i) {
        printf(" %s",stringArray[i]);
    }
    if(strMembership(od3, stringArray)){
        printf(" : Passed\n");
    } else{
        printf(" : failed\n");
    }

}

int main(){
    ODD odd;
    ODD odd2;
    ODD* result;
    readODD("even.txt", &odd);
    readODD("oddodd.txt", &odd2);

    result= unionODDs(&odd, &odd2);
    printODD("output.txt", result);

    char** stringArray = malloc(sizeof(char*)*5);
    for (int j = 0; j < 5; ++j) {
        stringArray[j] = malloc(sizeof(char)*2);
        stringArray[j][1] = 0;
    }

    char ar[] = {'a', 'b'};

    for (int i = 0; i < 2; ++i) {
        stringArray[0][0]=ar[i];
        for (int j = 0; j < 2; ++j) {
            stringArray[1][0]=ar[j];
            for (int k = 0; k < 2; ++k) {
                stringArray[2][0]=ar[k];
                for (int l = 0; l < 2; ++l) {
                    stringArray[3][0]=ar[j];
                    for (int m = 0; m < 2; ++m) {
                        stringArray[4][0]=ar[m];
                        printStringArrayStatus(result, stringArray);
                    }
                }
            }
        }
    }

    



    return 0;
}