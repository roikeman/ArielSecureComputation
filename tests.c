/*
 * tests.c
 *
 *  Created on: Mar 31, 2015
 *      Author: froike
 */
#include "secCompMultiParty.h"
#include <stdio.h>


int main(int argc, char **argv)
{
	char path[] = "/home/froike/cycle";

	Cycle* c = readCycleFromFile(path);
	printCycle(c);
	freeCircle(c);


	char a[]= "\tggggg\n";
	removeSpacesAndTubs(a);
	//printf("%s",a);


	return 0;

}
