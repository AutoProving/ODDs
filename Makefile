collapse_ODD: collapse_ODD.c 
	gcc -o collapse_ODD collapse_ODD.c sorting.c debugTools.c reading.c printing.c -g
sortRightIsBroken: sortRightIsBroken.c 
	gcc -o sortRightIsBroken sortRightIsBroken.c odd.h sorting.c debugTools.c reading.c printing.c -g
