// Copyright 2019 Christian Stafset, Daniel Lansdaal, Konstantin Rygol
// This file is licensed under MIT License, as specified in the file LISENSE located at the root folder of this repository.
#include <stdio.h>
#include <stdlib.h>
#include "odd.h"
#include "debugTools.h"
#include <omp.h> //also add -fopenmp to compile path
#include <string.h>

void minimize(ODD* o, ODD* result);

int findIndexState(StateContainer* v, State s) {
    //returns index i such that v[i]=s
    //binary search
    int bottom = 0;
    int top = v->nStates-1;
    int index = (bottom + top) / 2;
    int guess = v->set[index];
    while (guess != s) {
        if (s < guess) {
            top = index-1;
        }
        else {
            bottom = index+1;
        }
        index = (bottom + top) / 2;
        guess = v->set[index];
    }
    return index;
}

void cleanLeft(Layer* l);
void cleanRight(Layer* l);
void cleanTransitions(Layer* l);
void cleanTransitionsRight(Layer* l);
void copyStates(StateContainer* l1, StateContainer* l2);

ODD * collapseODD(ODD * odd);

void livingStateLeft(Layer * layer, StateContainer * livingStateLeft);

void livingStateRight(Layer * layer, StateContainer * livingStateRight);

void livingTransition(Layer * layer, TransitionContainer * livingTransition);

void livingFinalState(Layer * layer, StateContainer * livingFinalStates);

bool checkTypeLeft(Layer layer,State state_j,State state_k);

bool isFinalState(Layer layer, State state);

void minimizeLayer(Layer layer);

void minimizeFinalLayer(Layer layer);

void collapseLeft(Layer layer, State state_i, State state_j);

Transition * findTransitionLeft(TransitionContainer transitions, State state);

int main(int argc, char** argv) {
    ODD o;
    ODD res;

    readODD("ODD-ToMinimize2.txt", &o);
    showODD(&o);
    minimize(&o, &res);
}

void copyStates(StateContainer* l1, StateContainer* l2) {
    l2->nStates = l1->nStates;
    l2->set = memcpy(l2->set, l1->set, l1->nStates*sizeof(State));
    l2->set = realloc(l2->set, l2->nStates*sizeof(State));
}

int findIndexTransition(TransitionContainer T, Transition t) {
    //returns index i such that T[i] = t
}

int countUsefulStates(StateContainer s) {
    int n = 0;
    for (int i = 0; i < s.nStates; i++) {
        if (s.set[i] >= 0) {
            n++;
        }
    }
    return n;
}

void cleanTransitionsRight(Layer* l) {

	StateContainer* leftStates = &(l->leftStates);
	StateContainer* rightStates = &(l->rightStates);
	TransitionContainer* transitions = &(l->transitions);

	TransitionContainer auxTransitions;
	auxTransitions.set = malloc(transitions->nTransitions * sizeof(Transition));
	auxTransitions.nTransitions = transitions->nTransitions;
	for (int i = transitions->nTransitions - 1; i >= 0; i--) {
		auxTransitions.set[i].s2 = -1;
	}

	int i = 0;
	int curTransition = 0;
	int usefulTransitions = 0;

	for (int i = 0; i < rightStates->nStates; i++) {

		while (transitions->set[curTransition].s2 <= rightStates->set[i] && curTransition < transitions->nTransitions) {
			if (transitions->set[curTransition].s2 == rightStates->set[i]) {
				transitions->set[usefulTransitions] = transitions->set[curTransition];
				usefulTransitions++;
			}
			curTransition++;
		}
	}

	transitions->nTransitions = usefulTransitions;
	transitions->set = realloc(transitions->set, usefulTransitions * sizeof(Transition));

 //	for (int i = numLayers - 1; i >= 0; i--)
}

void cleanTransitions(Layer* l) {

    StateContainer* leftStates = &(l->leftStates);
    StateContainer* rightStates = &(l->rightStates);
    TransitionContainer* transitions = &(l->transitions);

    TransitionContainer auxTransitions;
    auxTransitions.set = malloc(transitions->nTransitions * sizeof(Transition));
    auxTransitions.nTransitions = transitions->nTransitions;
    for (int i = 0; i < transitions->nTransitions; i++) {
        auxTransitions.set[i].s1 = -1;
    }

    int i = 0;
    int curTransition = 0;
    int usefulTransitions = 0;

    for (int i = 0; i < leftStates->nStates; i++) {
        while (transitions->set[curTransition].s1 <= leftStates->set[i] && curTransition < transitions->nTransitions) {
            if (transitions->set[curTransition].s1 == leftStates->set[i]) {
                transitions->set[usefulTransitions] = transitions->set[curTransition];
                usefulTransitions++;
            }
            curTransition++;
        }
    }

    transitions->nTransitions = usefulTransitions;
    transitions->set = realloc(transitions->set, usefulTransitions * sizeof(Transition));

}

void cleanLeft(Layer* l) {

	StateContainer* usefulRight = &(l->rightStates);
	StateContainer* usefulLeft = &(l->leftStates);

	//fprintf(stderr, "accomplished, %i\n", usefulLeft->nStates);

	StateContainer auxStates;
	auxStates.set = malloc(usefulLeft->nStates * sizeof(State));
	auxStates.nStates = usefulLeft->nStates;
	for (int i = 0; i < usefulLeft->nStates; i++) {
		auxStates.set[i] = -1;
	}

	int transitionIndex = 0;
	int i = 0;
	//for each useful right state
	while (i < usefulRight->nStates) {
		while (l->transitions.set[transitionIndex].s2 <= usefulRight->set[i] && transitionIndex < l->transitions.nTransitions) {
			if (l->transitions.set[transitionIndex].s2 == usefulRight->set[i]) {
				// If useful, change value in aux state from -1
				State* s = &l->transitions.set[transitionIndex].s1;
				int index = findIndexState(usefulLeft, *s);
				auxStates.set[findIndexState(usefulLeft, *s)] = *s;
			}
			// check next transition
			transitionIndex++;
		}
		// go to next state
		i++;
	}
	int n = countUsefulStates(auxStates);

	usefulLeft->set = realloc(usefulLeft->set, n * sizeof(State));
	usefulLeft->nStates = n;
	int index = 0;

	for (int i = 0; i < auxStates.nStates; i++) {
		if (auxStates.set[i] != -1) {
			usefulLeft->set[index] = auxStates.set[i];
			index++;
		}
	}
}

void cleanRight(Layer* l) {

    StateContainer* usefulLeft = &(l->leftStates);
    StateContainer* usefulRight = &(l->rightStates);

    //fprintf(stderr, "accomplished, %i\n", usefulRight->nStates);
    
    StateContainer auxStates;
    auxStates.set = malloc(usefulRight->nStates * sizeof(State));
    auxStates.nStates = usefulRight->nStates;
    for (int i = 0; i < usefulRight->nStates; i++) {
        auxStates.set[i] = -1;
    }

    int transitionIndex = 0;
    int i = 0;
    //for each useful left state
    while (i < usefulLeft->nStates) {
        //for each transition with left state as current left state (or there is a dead state with no transition coming out which is smaller)
        while (l->transitions.set[transitionIndex].s1 <= usefulLeft->set[i] && transitionIndex < l->transitions.nTransitions) {
            //fprintf(stderr, "State Index: %i\nTransitionIndex: %i\nUseful Transitions: %i\n\n", i, transitionIndex);

            //if the state transition has current useful state as left state
            if (l->transitions.set[transitionIndex].s1 == usefulLeft->set[i]) {
                //set it as useful by changing aux state to not -1
                State* s = &l->transitions.set[transitionIndex].s2;
                int index = findIndexState(usefulRight, *s);
                auxStates.set[findIndexState(usefulRight, *s)] = *s;
            }
            //next transition
            transitionIndex++;
        }
        //next state
        i++;
    }

    int n = countUsefulStates(auxStates);

    usefulRight->set = realloc(usefulRight->set, n*sizeof(State));
    usefulRight->nStates = n;
    int index = 0;

    for (int i = 0; i < auxStates.nStates; i++) {
        if (auxStates.set[i] != -1) {
            usefulRight->set[index] = auxStates.set[i];
            index++;
        }
    }
}


ODD * collapseODD(ODD * odd){
    fprintf(stderr,"start collapsing ODD \n");
    //printf("start collapsing ODD");
    //create a new ODD and add layers to it
    int numLayers = odd->nLayers;
    //minimize final layer sort final layer after right states should be 
    void sortAllLeftTransitions(ODD* odd);
    minimizeFinalLayer(odd->layerSequence[numLayers-1]);
    //fprintf(stderr,"#final states in layer %d \n", odd->layerSequence[numLayers-1].finalStates.nStates);
    //showLayer(&(odd->layerSequence[numLayers-1]));
    //fprintf(stderr,"Minimzed final layer \n");
    //sort after the left states 
    
    //go backwards through ODD    
    for(int i = numLayers-1; i >= 0; i--)
    {
        //minize the left states                    //this is faulty
        //fprintf(stderr,"Number of left states %d \n",odd->layerSequence[i].leftStates.nStates);
        minimizeLayer(odd->layerSequence[i]);

      //  fprintf(stderr,"minimized left states in layer: %d \n",i);
        //collapse the right states accoardingly, The left states of Layer[i] 
        //corresponding to the right states of Layer[i-1]
        
        if (i != 0){ 
            for(int j = 0; j < odd->layerSequence[i-1].rightStates.nStates; j++)
            {
                odd->layerSequence[i-1].rightStates.set[j] 
                    = odd->layerSequence[i].leftStates.set[j]; 
            }
        }
        
        //reroute the transitions that point to the deleted right states 
        //to the states they collapsed to
        //if state is negative reroute to the abs() of negative number and shift by 1 
        for(int j = 0; j < odd->layerSequence[i].transitions.nTransitions; j++)
        {
            //we need the state to a transition or sorted transitions after the right states
            if(odd->layerSequence[i].rightStates.set[j] < 0)
            {
                odd->layerSequence[i].transitions.set[j].s2 
                    = -(odd->layerSequence[i].transitions.set[j].s2-1); 
            }
        }
        
       // fprintf(stderr,"Rerouted Transitions in layer: %d \n",i);
    //    fprintf(stderr,"Minimzed layer %d \n",i);
    }
   
    //build new ODD
    fprintf(stderr,"Build new odd \n");
    ODD * o = malloc(sizeof(ODD));
    o->width = odd->width;
  
    //we keep the number of Layers constant
    o->nLayers = odd -> nLayers;
    o->layerSequence = malloc(o->nLayers * sizeof(Layer));

    Layer l;
    //copy only states that are != -1 and their transitions
    for(int i = 0; i < numLayers; i++)
    {
        //the other variables of the struct are no dealt with in this function   
        livingStateLeft(&(odd->layerSequence[i]), &(l.leftStates));
        livingStateRight(&(odd->layerSequence[i]), &(l.rightStates));
        livingTransition(&(odd->layerSequence[i]), &(l.transitions));
        l.width = l.leftStates.nStates > l.rightStates.nStates ? 
            l.leftStates.nStates : l.rightStates.nStates;
        //coopy the other attributes from the old ODD
        l.map.N2S = malloc(sizeof(char*) * odd->layerSequence[i].map.sizeAlphabet); 
        l.map.S2N = malloc(sizeof(int) * odd->layerSequence[i].map.sizeAlphabet); 

        l.map.N2S = odd->layerSequence[i].map.N2S;
        l.map.S2N = odd->layerSequence[i].map.S2N;
        l.map.sizeAlphabet = odd->layerSequence[i].map.sizeAlphabet;
        l.initialFlag = odd->layerSequence[i].initialFlag;
        l.finalFlag = odd->layerSequence[i].finalFlag;
        l.initialStates.nStates = odd->layerSequence[i].initialStates.nStates;

        
        l.finalStates=odd->layerSequence[i].finalStates;
        //fprintf(stderr,"#final states in layer %d \n", odd->layerSequence[i].finalStates.nStates);
       //add the new layer to the new odd
        o->layerSequence[i] = l;
    }
    //fprintf(stderr, "Finished until final states \n"); 

    fprintf(stderr,"width of layer[0] of %d: %d \n", o->nLayers,o->layerSequence[0].width);
    o->layerSequence[0].initialStates=o->layerSequence[0].leftStates;

    //reset width of ODD to new width of ODD
    for(int i = 0; i < odd->nLayers; i++) {
        o->width = o->layerSequence[i].width > o->width ? 
            o->layerSequence[i].width : o->width;
    }
    //printf("The width of the new ODD is %d \n", o->width);
    fprintf(stderr,"This is the minimized ODD \n");
    
    //showODD(o);
    //free(&odd);
    //write the ODD to a file
    printODD("ODD-MySave.txt",o);
    return o;

}

void minimizeLayer(Layer layer){  
    //we minimize the left states and then return the new layer l  
    //we start with rightmost layer 
    //deletes unnecessary states and reroutes transitions
    //fprintf(stderr,"Number of left states %d \n",layer.leftStates.nStates);
    for(int j = 0; j < layer.leftStates.nStates;j++){  //assume states are sorted   
        State state_j = layer.leftStates.set[j];
        if (state_j >= 0){
            for(int k = j+1; k < layer.leftStates.nStates;k++){ //added +1 no point
                //in chekcing itelself 
                State state_k = layer.leftStates.set[k];
      //          fprintf(stderr,"j: %d, k: %d, state_j: %d, state_k: %d \n" , j,k,state_j,state_k);
                //if two states have only transitions that have the  same keys and right 
                //states collapse   
                if (checkTypeLeft(layer,state_j,state_k)){
                     fprintf(stderr,"check TypeLeft collapse for state_j:%d state_k:%d\n",
                        state_j,state_k);

                    collapseLeft(layer,state_j,state_k);    
                }
            } 
        }
    }
}

void minimizeFinalLayer(Layer layer){
    //we minimze the final layer no need to collapse transitions 
    //if both states are in the same final states group collapse them
    //fprintf(stderr,"Start minimize final layer \n");
    //fprintf(stderr,"#rightStates: %d \n", layer.rightStates.nStates );
    //state_j is either a final state or not a final state
    State state_i = layer.rightStates.set[0];
    bool fin_key_i = isFinalState(layer,state_i);
    //need to find the next state in the list that has the opposite final_state_property
    //of state_i we call this state state_j
    //after the func all states are collapsed to either state_i or state_j
    bool fin_key_j=fin_key_i;
    State state_j;
    int i=0;
    while(fin_key_i == fin_key_j){
        state_j = layer.rightStates.set[i];
        fin_key_j = isFinalState(layer,state_j);
        i++;
    }
    for(int i = 0; i < layer.rightStates.nStates; i++){
        State state_k = layer.rightStates.set[i];
        bool fin_key_k = isFinalState(layer,state_k);
        //fprintf(stderr,"i: %d, state_i %d ,fin_key_i %d , state_j %d, fin_key_j %d, state_k %dfin_key_k %d \n",i,state_i, fin_key_i, state_j, fin_key_j ,state_k,fin_key_k  );
        //do no try to collapse a state to itself
        if (state_k == state_i){
            layer.rightStates.set[i] = state_i; 
            continue;    
        }
        if(state_k == state_j){
            layer.rightStates.set[i] = state_j;   
            continue;
        }  

        if (fin_key_i == fin_key_k){
            layer.rightStates.set[i] = -(state_i+1); 
        }
        else {
            layer.rightStates.set[i] = -(state_j+1); 
        }
    }
    //The final living states are not entirely correct  
    layer.finalStates.nStates = 0; 
    if (fin_key_i){
        layer.finalStates.set[0] = state_i;
        layer.finalStates.nStates = 1; 
    }
    if (fin_key_j){
        layer.finalStates.set[0] = state_j;
        layer.finalStates.nStates = 1; 
    }
    //fprintf(stderr,"#final states %d \n", layer.finalStates.nStates);
    //showLayer(&layer);
}
    

bool checkTypeLeft(Layer layer,State state_j,State state_k){
    //j < k is implied if states are sorted
    Transition * trans_j = findTransitionLeft(layer.transitions,state_j);
    Transition * trans_k = findTransitionLeft(layer.transitions,state_k);
    int i = 0;

    //while left state stays the same = ntrans for a state
    while ((trans_j+i)->s1 == (trans_j+i+1)->s1)
    { 
        NumSymbol key_j = (trans_j+i)->a;
        State state_right_j = (trans_j+i)->s2;
        
        NumSymbol key_k = (trans_k+i)->a;
        State state_right_k = (trans_k+i)->s2;
     
       //then do not collapse
        if(key_j != key_k || state_right_j != state_right_k){
            return 0; //not collapse
        }
        i++;                
    }
    if(i > 1){
        return 1; //collapse
    }
    else{
        return 0; // do no collapse
    }
}

bool isFinalState(Layer layer, State state){
//return 1 if state is part of final states 
//call a binary search on Final states.set
    int left = -1;
    int right = layer.finalStates.nStates;

    while (right > left +1){
        int middle = (left+right)/2;
        //cout << "middle" << middle << endl;
        if (layer.finalStates.set[middle] >= state){
            right = middle;
        }
        else{
            left = middle;
        }
    }
    if (right < layer.finalStates.nStates && layer.finalStates.set[right] == state){
        return 1;//
    }
    else{
        //state is not part of final states 
        return 0;
    }


}

void collapseLeft(Layer layer, State state_i, State state_j){
    //sets the deleted state to -(upperstate+1) the transitions to the lower state will be deleted
    if(state_i > state_j){ //to ensure that state_i < state_j 
        State tmp =  state_i;
        state_i = state_j;
        state_j = tmp;
        }
    //delete state j and say where it get rerouted to save it as negative-1 to account for state 0 
    layer.leftStates.set[state_j] = -(state_i+1); 
    //collapse all transitions that start in state_j to the state_i
    //both have the same number of Transitions so we could also take state_j
    //we set the transition of the to be deleted state to -1
    int i = 0;
    Transition * trans_j = findTransitionLeft(layer.transitions,state_j);
    //buffer the left state that is to be overwritten
    int buff = trans_j->s1;
    int buff_incr = (trans_j+i)->s1;
    while(buff == buff_incr)
    {
        buff_incr = (trans_j+i)->s1;
        (trans_j+i)->s1 = -1;
        i++;
    }
       
}

//returns the first transition of a state 
Transition * findTransitionLeft(TransitionContainer transitions, State state){
    //finds the index of first occurence of a state in the sorted transitions list
    int left = -1;
    int right = transitions.nTransitions;

    while (right > left +1){
        int middle = (left+right)/2;
        //cout << "middle" << middle << endl;
        if (transitions.set[middle].s1 >= state){
            right = middle;
        }
        else {
            left = middle;
        }
    }
    if (right < transitions.nTransitions && transitions.set[right].s1 == state){
        return &(transitions.set[right]);
    }
    else{
      //  printf("state is not part of transition list \n");
        return &(transitions.set[right]);
    }

}

void livingStateLeft(Layer * layer, StateContainer * livingStateLeft){
    int count = 0;
    for(int i = 0; i < layer->leftStates.nStates;  i++)
    {
        if(layer->leftStates.set[i] >= 0) 
            count++;
    }
    livingStateLeft->set = malloc(count * sizeof(State));
    livingStateLeft->nStates = count;
    count = 0;
    for(int i = 0; i < layer->leftStates.nStates; i++)
    {
        if(layer->leftStates.set[i] >= 0){
            livingStateLeft->set[count] = layer->leftStates.set[i];
            count++;
        }
    }
}

void livingStateRight(Layer * layer, StateContainer * livingStateRight){
    int count = 0;
    for(int i = 0; i < layer->rightStates.nStates;  i++)
    {
        if(layer->rightStates.set[i] >= 0)
            count++;
    }
    livingStateRight->set = malloc(count * sizeof(State));
    livingStateRight->nStates=count;
    count = 0;
    for(int i = 0; i < layer->rightStates.nStates; i++)
    {
        if(layer->rightStates.set[i] >= 0){
            livingStateRight->set[count] = layer->rightStates.set[i];
            count ++;
        }
    }
}

void livingTransition(Layer * layer, TransitionContainer * livingTransition){
    int count = 0;
    for(int i = 0; i < layer->transitions.nTransitions;  i++)
    {
        if(layer->transitions.set[i].s1  >= 0)
            count++;
    }
    livingTransition->set = malloc(count * sizeof(Transition));
    livingTransition->nTransitions=count;
    count = 0;
    for(int i = 0; i < layer->transitions.nTransitions; i++)
    {
        if(layer->transitions.set[i].s1  >= 0){
            livingTransition->set[count] = layer->transitions.set[i];
            count ++;
        }
    }
}

void minimize(ODD* o, ODD* result) {

    int numLayers = o->nLayers;

    copyStates(&(o->layerSequence[0].initialStates), &(o->layerSequence[0].leftStates));

        fprintf(stderr, "BEGIN:\n\n");
    for (int i = 0; i < numLayers; i++) {
        cleanTransitions(&(o->layerSequence[i]));
        cleanRight(&(o->layerSequence[i]));
        if (i < numLayers - 1) {
            copyStates(&(o->layerSequence[i].rightStates), &(o->layerSequence[i+1].leftStates));
        }
        
    }
    	fprintf(stderr, "BEGIN RtL:\n\n");
        sortAllRightTransitions(o);
	for (int i = numLayers - 1; i >= 0; i--){
		cleanLeft(&(o->layerSequence[i]));
		cleanTransitionsRight(&(o->layerSequence[i]));

		if (i > 0) {
			copyStates(&(o->layerSequence[i].leftStates), &(o->layerSequence[i-1].rightStates));
		}
	}
    //sortAllLeftTransitions(o);

	result = collapseODD(o);
}


