#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "odd.h"
//#include "debugTools.h"
using namespace std;


Transition * findTransitionLeft(TransitionContainer transitions, State state){
    //finds the first occurence of a state in the sorted transitions list
    //returns then addres of this transition
    int left = -1;
    int right = transitions.nTransitions;

    while (right > left +1){
        int middle = (left+right)/2;
        cout << "middle" << middle << endl;
        if (transitions.set[middle].s1 >= state){
            right = middle;
        }
        else{
            left = middle;
        }
    }
    if (right < transitions.nTransitions && transitions.set[right].s1 == state){
        cout << right << endl;
        return &(transitions.set[right]);
        
    }
    else{
        printf("state is not part of transition list \n");
        return &(transitions.set[right]);
    }

}

int main(){
 
    
    //TransitionContainer * test_container = (TransitionContainer *) 
     //   malloc(10* sizeof(Transition));
    
    TransitionContainer test_container;
    test_container.set = (Transition *) malloc(10 * sizeof(Transition));

    //the test container is sorted after left state, numSymbol, right state 
    test_container.nTransitions = 10;
    test_container.set[0].s1=0;
    for(int i = 0; i < 10;i++)//test_container.nTransitions ; i++)
    {
        test_container.set[i].s1=i/3;
        test_container.set[i].a=(i+3)%10;
        test_container.set[i].s2=(i+7)%2;
        cout << test_container.set[i].s1 << " " << test_container.set[i].a << " " <<  test_container.set[i].s2 << endl;

    }
    //sortLeftTransitions(&test_container);
    State state_j=2;
    Transition *corr_trans = findTransitionLeft(test_container,state_j);


    cout << corr_trans ->s1 << " " << corr_trans->a << " "<< corr_trans -> s2 << endl;
    
    
    return 0;
    
}