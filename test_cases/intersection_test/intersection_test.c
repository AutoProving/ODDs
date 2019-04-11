// Copyright 2019 Daniel Notland
// This file is licensed under MIT License, as specified in the file LICENSE located at the root folder of this repository.


#include <stdio.h>
#include <stdlib.h>
#include "odd.h"
#include "debugTools.h"
#include "cloneKill.h"

int oddIntersectEvenTest(ODD* odd1, ODD* odd2, int parallel);
int evenIntersectMod4Test(ODD* odd1, ODD* odd2, int parallel);

int main(){
    printf("---{Testing intersection}---\n");

    // LOADING EXAMPLE ODDS
    ODD evenODD;
    readODD("ODD1-Example.txt", &evenODD);

    ODD oddODD;
    readODD("ODD2-Example.txt", &oddODD);

    ODD mod4ODD;
    readODD("ODD-Example-MOD4.txt", &mod4ODD);

    printf("\n---{Sequential testing}---\n");
    if(oddIntersectEvenTest(&oddODD, &evenODD, 0) == 1)  {
        printf("-odd intersect even test: passed\n");
    } else {
        printf("-odd intersect even test: failed\n");
    }

    if(evenIntersectMod4Test(&evenODD, &mod4ODD, 0) == 1)  {
        printf("-even intersect mod4 test: passed\n");
    } else {
        printf("-even intersect mod4 test: failed\n");
    }

    printf("\n---{Parallel testing}---\n");
    if(oddIntersectEvenTest(&oddODD, &evenODD, 1) == 1)  {
        printf("-odd intersect even test: passed\n");
    } else {
        printf("-odd intersect even test: failed\n");
    }

    if(evenIntersectMod4Test(&evenODD, &mod4ODD, 1) == 1)  {
        printf("-even intersect mod4 test: passed\n");
    } else {
        printf("-even intersect mod4 test: failed\n");
    }


    return 0;
}


int oddIntersectEvenTest(ODD *odd1, ODD *odd2, int parallel) {
    int* testData = (int*) malloc(sizeof(int) * 5);
    int testPassed = 1;
    ODD* iODD;
    if(parallel == 1) {
        iODD = intersectionODDParallel(odd1, odd2);
    } else  {
        iODD = intersectionODD(odd1, odd2);
    }

    for(int i = 0; i < 2; i++) {
        testData[0] = i;
        for(int i1 = 0; i1 < 2; i1++) {
            testData[1] = i1;
            for(int i2 = 0; i2 < 2; i2++) {
                testData[2] = i2;
                for(int i3 = 0; i3 < 2; i3++) {
                    testData[3] = i3;
                    for(int i4 = 0; i4 < 2; i4++) {
                        testData[4] = i4;
                        if(numMembership(iODD, testData) == 1) {
                            testPassed = 0;
                            printf("Should not accept string:\n");
                            for(int a = 0; a < 5; a++) {
                                printf("%i ", testData[a]);
                            }
                            printf("\n\n");
                        }
                    }
                }
            }
        }
    }
    free(testData);
    //killODD(iODD);
    return testPassed;
}

int evenIntersectMod4Test(ODD* odd1, ODD* odd2, int parallel) {
    int* testData = (int*) malloc(sizeof(int) * 5);
    int testPassed = 1;
    ODD* iODD;
    if(parallel == 1) {
        iODD = intersectionODDParallel(odd1, odd2);
    } else  {
        iODD = intersectionODD(odd1, odd2);
    }
    for(int i = 0; i < 2; i++) {
        testData[0] = i;
        for(int i1 = 0; i1 < 2; i1++) {
            testData[1] = i1;
            for(int i2 = 0; i2 < 2; i2++) {
                testData[2] = i2;
                for(int i3 = 0; i3 < 2; i3++) {
                    testData[3] = i3;
                    for(int i4 = 0; i4 < 2; i4++) {
                        testData[4] = i4;
                        int sum = testData[0] + testData[1] + testData[2] +testData[3] + testData[4];
                        if(numMembership(iODD, testData) == 1 && sum != 4) {
                            testPassed = 0;
                            printf("Should not accept string:\n");
                            for(int a = 0; a < 5; a++) {
                                printf("%i ", testData[a]);
                            }
                            printf("\n\n");
                        }
                    }
                }
            }
        }
    }
    free(testData);
    //killODD(iODD);
    return testPassed;
}

