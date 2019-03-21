// Copyright 2019 Christian Stafset, Daniel Lansdaal, Konstantin Rygol
// This file is licensed under MIT License, as specified in the file LISENSE located at the root folder of this repository.
#include <stdio.h>
#include <stdlib.h>
#include "odd.h"
#include "debugTools.h"
#include <omp.h> //also add -fopenmp to compile path


/*

int main() {
    usefulLeft = initial states of first layer
    for i = 0 ... nTransitions-1 {
        Layer l = cleanRight(odd_layers[i], usefulLeft, usefulRight)
    }
}

void cleanRight (Layer* l, State* usefulLeft (assume ordered), State* usefulRight) {
    layer->left = usefulLeft;
    int usefulTransitions = 0;
    int stateCounter = 0;
    for (i = 0...N transitions-1) {
        if (layer.transitions.set[i].s1 belongs to useful left) {
            add layer.transitions[i].right to useful right
            usefulTransitions++
        }
    }
    layer.rightstates = usefulright;
    Transition* aux
    allocate space for n useful transitions in array aux
    int transitioncounter = 0;
    for i = 0 ... ntransitions-1 {
        if layertransitions[i].left belongs to useful left {
            aux[transitioncounter] = transition
        }
    }
}






*/






Layer removeTransitions(Layer l);

Layer setUpInitialLayer(Layer l) {
    //remove dead left states in first layer and put it in new layer
    Layer newLayer;
    newLayer.leftStates = l.initialStates;
    newLayer.initialStates = l.initialStates;
    newLayer.finalFlag = l.finalFlag;
    newLayer.finalStates = l.finalStates;
    newLayer.initialFlag = l.initialFlag;
    newLayer.map = l.map;
    newLayer.rightStates = l.rightStates;
    newLayer.width = l.width;
    fprintf(stderr, "problem?");
    newLayer = removeTransitions(newLayer);
    fprintf(stderr, "now?");

    //free(&l);
    return newLayer;

}

Layer removeDeadRightStates(Layer l) {
   //go through all right states
   State* rightStates = l.rightStates.set;
   int numStates = l.rightStates.nStates;

   Transition* transitions = l.transitions.set;
   int numTransitions = l.transitions.nTransitions;

   long count = 0;
   //go through transitions until find that state
   for (int i = 0; i < numStates; i++) {
       bool found = false;
       for (int j = 0; j < numTransitions; j++) {
           //if you find it, change boolean
           if (rightStates[i] == transitions[j].s2) {
               found = true;
               break;
           }
       }
       if (found) {
           count++;
       }
       else {
           rightStates[i] = -1;
       }
   }

    State* newStates = malloc (count * sizeof(State));
    count = 0;
    for (int i = 0; i < numStates; i++) {
        if (rightStates[i] != -1) {
            newStates[count] = rightStates[i];
            count++;
        }
    }

    free(l.rightStates.set);
    l.rightStates.set = newStates;
    //free(&(l.rightStates.nStates));
    l.rightStates.nStates = count;

    return l;
   
}

Layer removeTransitions(Layer l) {
    Transition* transitions = l.transitions.set;
    int numTransitions = l.transitions.nTransitions;

    State* leftStates = l.leftStates.set;
    int numStates = l.leftStates.nStates;

    fprintf(stderr, "after here?");

    long count = 0;
    //go through all transitions
    for (int i = 0; i < numTransitions; i++) {
        bool valid = false;
        //go through all states and check if s1 of transition is in states
        for (int j = 0; j < numStates; j++) {
            if (transitions[i].s1 == leftStates[j]) {
                //if it is change boolean
                valid = true;
                break;
            }
            fprintf(stderr, "that for loop?");
        }
        //if valid increase count
        if (valid) {
            count++;
        }
        //otherwise set s1 to -1 so we dont add it
        else {
            fprintf(stderr, "this must be it %i", i);
            transitions[i].s1 = -1;
        }
        fprintf(stderr, "do you get here?");
    }

    fprintf(stderr, "whats wrong?");

    Transition* newTrans = malloc(count * sizeof(Transition));
    count = 0;
    for (int i = 0; i < numTransitions; i++) {
        if (transitions[i].s1 != -1) {
            //add
            newTrans[count] = transitions[i];
            count++;
        }
    }

    free(l.transitions.set);
    l.transitions.set = newTrans;
    //free(&(l.transitions.nTransitions));
    l.transitions.nTransitions = count;
    
}


ODD* removeDeadStatesLeftToRight(ODD* odd) {
    //create new odd to return
    ODD* newOdd = malloc(sizeof(ODD));
    newOdd->nLayers = odd->nLayers;
    newOdd->layerSequence = malloc(newOdd->nLayers * sizeof(Layer));

        fprintf(stderr, "%i\n", newOdd->nLayers);
    //delete dead initial states and transitions
    Layer l1 = odd->layerSequence[0];
    fprintf(stderr, "ello");
    newOdd->layerSequence[0] = setUpInitialLayer(odd->layerSequence[0]);
    fprintf(stderr, "govner");
    for (int i = 0; i < newOdd->nLayers-1; i++) {
        //check right states
        fprintf(stderr, "a\n");
        Layer cur = newOdd->layerSequence[i];
        //delete dead right states
        cur = removeDeadRightStates(cur);
        fprintf(stderr, "b\n");
        newOdd->layerSequence[i] = cur;
        //new layer
        cur = odd->layerSequence[i+1];
        //set left states to right states of previous layer
        cur.leftStates = newOdd->layerSequence[i].rightStates;
        //delete transitions
        cur = removeTransitions(cur);
        fprintf(stderr, "c\n");
        newOdd->layerSequence[i+1] = cur;
        //repeat
    }

    free(odd);

    return newOdd;

}


// int main(int argc, char** argv) {
//     ODD* odd = malloc(sizeof(ODD));
//     readODD("ODD-Example.txt", odd);
    
//     showODD(odd);

//     odd = removeDeadStatesLeftToRight(odd);

//     showODD(odd);
// }





Layer minimizeLayer(Layer * layer,Layer l,int i){ //why do we need int i ?  
    #pragma omp parallel for
    for(int j = 0; j < layer->leftStates.nStates;j++){  //assume states are sorted   
        State* state_j = layer->leftStates.set[j];
        if (state_j != -1){
            for(int k = j; k < layer->leftStates.nStates;j++){ //not sure about that ??
                State* state_k = layer->leftStates.set[k];
                //check whether j and k are of same type if true they have the same type  
                if (checkType(&state_j,&state_k)){
                    collapseLeft(&layer,&state_j,&state_k);   //where do we put right collapse ?  
                }
            } 
        }
    }
    return l;
}

//takes tranitions not states as we also need NumSymbol
bool checkTypeLeft(Transition * transition_j,Transition * transition_k){
    if(transition_j->s1 == transition_k->s1)
        if(transition_j->a == transition_k->a)
            return 1;
    return 0;
}
bool checkTypeRight(Transition * transition_j,Transition * transition_k){
    if(transition_j->s2 == transition_k->s2)
        if(transition_j->a == transition_k->a)
            return 1;
    return 0;
}

void collapseLeft(Layer * layer, Transition * t_i, Transition * t_j){
    State state_i;
    State state_j;
    if(t_i->s1 < t_j->s1){ //to ensure that state_i < state_j 
        State tmp =  t_i->s1;
        state_i = t_i->s1;
        state_j = tmp;
        }

    layer->leftStates.set[state_i] = -1;
    //get num symbol of state to be replaced -> go to left state, 

    //DANIEL changed the n in this for loop to nTransitions. Think its right?

    for (int i = 0; i < layer->transitions.nTransitions; i++){ //to what does this go ? This is all transitions from 
        //state x to state j. We assume the states are sorted
        
        //Transition.s1[state_j] = Transition.s1[state_i];
    }    
}

//void collapse right
void collapseRight(Layer * layer, State * state_i, State * state_j){
    if(state_i < state_j){ //to ensure that state_i < state_j 
        State tmp =  state_i;
        state_i = state_j;
        state_j = tmp;
        }

    layer->rightStates.set[*state_j] = -1;
    //get num symbol of state to be replaced -> go to left state, 
    for (int i = 0; i < layer->transitions.nTransitions; i++){ //to what does this go ? This is all transitions from 
        //state x to state j. We assume the states are sorted

        //Transition.s2[state_j] = Transition.s2[state_i];
    }    
}







//need to go twice, forwards and backwards to remove states which have nothing from the left and nothing from the right

ODD* minimize (ODD* odd) {

    // //create copy of ODD, I guess this is how we copy anything we need to 
    // ODD* p = &odd;
    // ODD newODD = *p;

    //maybe this is better to define new ODD
    ODD* o;
    o->nLayers = odd->nLayers;

    o->layerSequence = malloc(o->nLayers * sizeof(Layer));

    //better to create new ODD and add layers to it like we thought orignally LOL

    int numLayers = odd->nLayers;

    //create new layer then we need to free the old one before replacing it with the new one we created
    Layer l;
    //change things in the layer using '.' e.g.
    l.width = odd->layerSequence[0].width;

    Layer currLayer = odd->layerSequence[0];

    State* livingStates = currLayer.leftStates.set;
    int livingCount = currLayer.leftStates.nStates;

    Transition* livingTransitions =  currLayer.transitions.set;
    int livingTransCount = currLayer.transitions.nTransitions;

    o = removeDeadStatesLeftToRight(odd);

    //reset width to new width
    for(int i = 0; i < odd->nLayers; i++) {
        odd->width = odd->layerSequence[i].width > odd->width ? odd->layerSequence[i].width : odd->width;
    }

    //my part starts here the upper part is from daniel
    //we look at new type it is the kartisian product {Numbsymbol} x {left/right_state}
    //sort the transitions do we need to do that here ?? 
    void sortAllLeftTransitions(ODD* odd);
    void sortAllRightTransitions(ODD* odd);
    
    for (int i = 0; i < odd->nLayers; i++){
        l = minimizeLayer(odd->layerSequence,odd->layerSequence[i], i);    
        //get new trimmed Layer without the -1 states 
        
    }



    //reset width to new width
    for(int i = 0; i < odd->nLayers; i++) {
        odd->width = odd->layerSequence[i].width > odd->width ? odd->layerSequence[i].width : odd->width;
    }




    //free original ODD?
    free(odd);

    return o;
}
