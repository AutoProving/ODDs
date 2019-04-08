//
// Created by sweetsleep on 4/8/19.
//
#include "../../odd.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

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
    ODD od1;
    ODD od2;
    ODD od3;
    readODD("odd1.txt", &od1);
    readODD("odd2.txt", &od2);

    od3 = *tensorODD(&od1, &od2);
    printODD("out.txt", &od3);

    char** stringArray = malloc(sizeof(char*)*5);
    for (int j = 0; j < 5; ++j) {
        stringArray[j] = malloc(sizeof(char)*4);
        stringArray[j][0] = 'a';
        stringArray[j][1] = '|';
        stringArray[j][2] = 'b';
        stringArray[j][3] = 0;
    }

    char ar[] = {'a', 'b'};

    for (int i = 0; i < 2; ++i) {
        stringArray[0][0]=ar[i];
        for (int j = 0; j < 2; ++j) {
            stringArray[0][2]=ar[j];
            for (int k = 0; k < 2; ++k) {
                stringArray[1][0]=ar[k];
                for (int l = 0; l < 2; ++l) {
                    stringArray[1][2]=ar[j];
                    for (int m = 0; m < 2; ++m) {
                        stringArray[2][0]=ar[m];
                        for (int n = 0; n < 2; ++n) {
                            stringArray[2][2]=ar[n];
                            for (int i1 = 0; i1 < 2; ++i1) {
                                stringArray[3][0]=ar[i1];
                                for (int j1 = 0; j1 < 2; ++j1) {
                                    stringArray[3][2]=ar[j1];
                                    for (int k1 = 0; k1 < 2 ; ++k1) {
                                        stringArray[4][0]=ar[k1];
                                        for (int l1 = 0; l1 < 2; ++l1) {
                                            stringArray[4][2]=ar[l1];
                                            printStringArrayStatus(&od3, stringArray);

                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    printStringArrayStatus(&od3, stringArray);



    return 0;
}