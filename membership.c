// Copyright 2019 YOUR NAME
// This file is licensed under MIT License, as specified in the file LISENSE located at the root folder of this repository.

#include <odd.h>
#include <stdio.h>
#include <stdbool.h>

using namespace std;

bool numMembership(ODD* odd, int* numSeq){

    Layer currentLayer;
    State currentState;
    bool noTransitionFound;


    //Try all initial states
    for(int i=0; i< odd.layers[0].initialStates.nStates; i++){

    }

    //Try all initial states
    for(state s : odd.layers[0].initialStates.set){
        bool b = tryPath(odd, 0, s, odd.layerSequence[0], 0);
        if(b){
            return 1;
        }
    }
    return 0;
}

bool tryPath(ODD* odd, int* numSeq, State state, Layer layer, int seq){
    for(transition t : layer.transition.set){
        if(t.s1 == state && t. == numSeq[seq]){
            bool b = tryPath(odd, numSeq, t.s2, odd.layerSequence[seq+1], seq+1);
            if(b){
                return 1;
            }
        }
    }
    return 0;
}

bool strMembership(ODD* odd, char** strSeq){

    State currentState = odd.InitialStates.set[i];
    Layer currentLayer;
    AlphabetMap currentMap;
    int currentInt;
    int partition;
    int lowerBound;
    int upperBound;
    bool noTransitionFound;

    for(int i = 0; i < odd.nLayers; i++){
        currentLayer = odd.LayerSequence[i];
        currentMap = currentLayer.map;


        lowerBound = 0;
        upperBound = currentMap.sizeAlphabet;
        partition = (upperBound + lowerBound) / 2;
        while(true){
            if(currentMap.N2S[currentMap.S2N[partition]] == strSeq[i]){
                currentInt = currentMap.S2N[partition];
                break;
            }

            //No map forund
            else if(upperBound == lowerBound){
                    return 0;
                }
            else if(currentMap.N2S[currentMap.S2N[partition]] < strSeq[i]){
                lowerBound = partition;
            }
            else{
                upperBound = partition;
            }
            partition = (upperBound + lowerBound) / 2;

        }

        noTransitionFound = True;
        //Find transition
        for(transition t : currentLayer.transitions.set){
            if(t.a == currentInt && t.s1 == currentState){
                currentState = t.s2;
                noTransitionFound = False;
                break;
            }
        }

        if(noTransitionFound){
            return 0;
        }
    }


    return 1;
}