// Copyright 2019 Josep Barbera Muñoz
// This file is licensed under MIT License, as specified in the file LISENSE located at the root folder of this repository.

#include <stdlib.h>
#include <studio.h>
#include <odd.h>

bool text_complete_layer(layer b)
{
	
	int count=0;
	
	/*I dont know if i need count the total of all the layers that is the reason of the comented for*/
	/*for(int j==0; j <= total_layers; j++)*/
	
	for (int i==0; i<layer->size;i++)
	{
		
		count ++;
		
		if(count==alphabet->size)
		{
			
			return true;
			
		}
	}
/*}*/
	return false;
}

bool test_complete_ODD(ODD d)
{
	
	bool compl[total_layers];
	
	for(int i == 0; i<ODD->size; i++)
	{
		
		compl = text_complete_layer(layer b);
		
		if (compl[0]==true && compl[1]==true && compl[2]==true && compl[3]==true)
		{
			
			return true;
			
		}
		if(compl[0]==false && compl[1]==false && compl[2]==false && compl[3]==false)
		{
			
			return false;
			
		}
	}
}
