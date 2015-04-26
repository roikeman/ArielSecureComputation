/*
 * secCompMultiParty.c
 *
 *  Created on: Mar 31, 2015
 *      Author: froike
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "secCompMultiParty.h"

Gate GateCreator(const unsigned int inputBit1, const unsigned int inputBit2, const unsigned int outputBit, const unsigned int TTable, const unsigned int flags)
{
	Gate g = {.inputBit1 = inputBit1, .inputBit2 = inputBit2, .outputBit = outputBit, .truthTable = TTable, .flags = flags};
	return g;
}


Cycle * readCycleFromFile(char * path)
{
	unsigned int gateAmount = 0;
	unsigned int playerAmount = 0;
	unsigned int lineCount = 1;
	unsigned int playerCounter = 0;
	unsigned int gateCounter = 0;

	unsigned int specialGatesAmount = 0;

	unsigned int i;

	unsigned int tempPlayerID;
	unsigned int tempAmountOfBits;
	unsigned int tempInput1;
	unsigned int tempInput2;
	unsigned int tempOutput;
	unsigned int tempTTable;
	unsigned int tempStatus;

	unsigned int gotOutputsBits = false;


	Cycle * cycleTR;
	char lineBuff[STRING_BUFFER_SIZE];
	char lineBuffCopy[STRING_BUFFER_SIZE];

	FILE * cycleFile = fopen(path, "r");

	if (cycleFile == NULL){
		printf("Error: Bad file path...");
		return NULL;
	}

	if(!fgets(lineBuff, STRING_BUFFER_SIZE, cycleFile)){
		printf("Error: Seems to be empty file...");
		return NULL;
	}




	while(fgets(lineBuff, STRING_BUFFER_SIZE, cycleFile)){
		//check for notes lines or empty lines. if found, continue.
		strcpy(lineBuffCopy,lineBuff);
		removeSpacesAndTubs(lineBuffCopy);
		if(lineBuffCopy[0] == 0 || lineBuffCopy[0] == '#' || lineBuffCopy[0] == '\n' ) continue ;

		//will append in the first line and just one time.
		if(gateAmount == 0){
			if(sscanf(lineBuff,"%u %u", &gateAmount, &playerAmount)!=2){
				printf("Error: First line in the file had to be in format of '<amount of gates> <amount of players>'\ne.g '32123 3'");
				return NULL;
			}
			if(gateAmount <= 0){
				printf("Error: Amount of gates need to be more than 0");
				return NULL;
			}
			// Init Circle struct
			cycleTR = (Cycle*) malloc(sizeof(Cycle));
			cycleTR -> gateArray = (Gate *) malloc(sizeof(Gate)*gateAmount);
			cycleTR -> playerArray = (Player **) malloc(sizeof(Player*)*playerAmount);
			for (i = 0; i < playerAmount; ++i) {
				cycleTR -> playerArray[i] = NULL;
			}
			cycleTR -> amountOfGates = gateAmount;
			cycleTR -> amountOfPlayers = playerAmount;

		}else if (playerCounter < playerAmount){
			if (sscanf(lineBuff,"P%u %u",&tempPlayerID,&tempAmountOfBits) == 2){			//
				cycleTR -> playerArray[tempPlayerID] = (Player*) malloc(sizeof(Player));	//   Init new player....
				cycleTR -> playerArray[tempPlayerID] -> playerBitAmount = tempAmountOfBits;	//
				cycleTR -> playerArray[tempPlayerID] -> playerBitArray = (unsigned int*) malloc(sizeof(unsigned int)*tempAmountOfBits);
				//printf("amount: %u",tempAmountOfBits);
				for (i = 0; i < tempAmountOfBits; ++i) {
					if (!fgets(lineBuff, STRING_BUFFER_SIZE, cycleFile)) { printf("Error: in line %u bit serial expected... but the file is ended.", lineCount); return NULL;}
					lineCount++;

					if(sscanf(lineBuff,"%u",&cycleTR -> playerArray[tempPlayerID] -> playerBitArray[i]) != 1) {printf("Error: in line %u expected for bit serial... ", lineCount); return NULL;}

				}
				playerCounter++;
			}else{
				printf("Error: Player header expected. e.g P1 32");
				return NULL;
			}

		}else if(!gotOutputsBits){
			gotOutputsBits = true;
			if (sscanf(lineBuff,"Out %u",&tempAmountOfBits) == 1){
				cycleTR -> outputbits.playerBitAmount = tempAmountOfBits;
				cycleTR -> outputbits.playerBitArray = (unsigned int*) malloc(sizeof(unsigned int)*tempAmountOfBits);
				for (i = 0; i < cycleTR -> outputbits.playerBitAmount; ++i) {
					if (!fgets(lineBuff, STRING_BUFFER_SIZE, cycleFile)) { printf("Error: in line %u bit serial expected... but the file is ended.", lineCount); return NULL;}
					lineCount++;
					if(sscanf(lineBuff,"%u",&cycleTR -> outputbits.playerBitArray[i]) != 1) {printf("Error: in line %u expected for bit serial... ", lineCount); return NULL;}
				}

			}else{
				printf("Error: Outputs header expected. e.g O 32");
				return NULL;
			}




		}else{//if done with Players, lets collect gates
			if ((tempStatus = sscanf(lineBuff,"%u %u %u %u %s\n",&tempInput1,&tempInput2,&tempOutput,&tempTTable)) >=4){

				/** if(tempStatus >= 5){
					if(strcmp(lineBuffCopy,dispFreeNor) == 0) tempStatus = flagFreeNor;
					else if(strcmp(lineBuffCopy,dispFreeXor) == 0) tempStatus = flagFreeXor;
					else  tempStatus = flagNone;
				}else tempStatus = flagNone; **/

				if(tempTTable == 110) {
					tempStatus = flagXor;
					specialGatesAmount++;
				}else if(tempTTable == 1001){
					tempStatus = flagXnor;
					specialGatesAmount++;
				}else tempStatus = flagNone;

				cycleTR-> gateArray[gateCounter] = GateCreator(tempInput1, tempInput2, tempOutput, tempTTable, tempStatus);


				gateCounter++;
			}else{
				printf("Error: Gate header expected.. format: 1 2 3 1001");
				return NULL;
			}
		}

		lineCount++;
	}

	if (gateCounter < gateAmount) {
		printf("Error: expected to %u gates, but got only %u...",gateAmount,gateCounter);
		return NULL;
	}

	cycleTR -> specialGates = specialGatesCollector(cycleTR->gateArray,gateAmount,specialGatesAmount);
	printf("test: %u",specialGatesAmount);
	return cycleTR;
}


void printCycle(const Cycle * c)
{
	char * flagsFriendlyNames[] = {"",dispXor,dispXnor};
	int p,i;
	if (c == NULL) {
		printf("Error: got NULL...\n");
	}else{
		printf("Gates amount: %u, players: %u\n",c->amountOfGates,c->amountOfPlayers);
		for(p = 0; p < c->amountOfPlayers; p++ ){
			printf("P%d %u\n",p,c->playerArray[p]->playerBitAmount);
			for (i = 0; i < c->playerArray[p]->playerBitAmount; ++i) {
				printf("%u\n",c->playerArray[p]->playerBitArray[i]);
			}
		}
		printf("Out %u\n",c->outputbits.playerBitAmount);
		for (i = 0; i < c->outputbits.playerBitAmount; ++i) {
			printf("%u\n",c->outputbits.playerBitArray[i]);
		}
		for(i=0;i<c->amountOfGates;i++){
			printf("%u %u %u %u %s\n",c->gateArray[i].inputBit1,c->gateArray[i].inputBit2,c->gateArray[i].outputBit,c->gateArray[i].truthTable,flagsFriendlyNames[c->gateArray[i].flags]);
		}
	}
}

void removeSpacesAndTubs(char* source)
{
	char* i = source;
	char* j = source;
	while(*j != 0)
	{
		*i = *j++;
		if(*i != ' ' && *i != '\t')
			i++;
	}
	*i = 0;
}

void freeCircle(Cycle * c)
{
	if (!c){
		printf("Error: expected for circle , but got NULL...");
		return;
	}

	int i;
	for (i = 0; i < c->amountOfPlayers; ++i) {
		free(c->playerArray[i]->playerBitArray);
	}
	free(c->playerArray);
	free(c->gateArray);
	free(c);
}

Gate ** specialGatesCollector(Gate * GatesArray, const unsigned int arraySize, const unsigned int specialGatesAmount){
	unsigned int i;
	unsigned int count = 0;
	Gate ** sarray = (Gate**) malloc(sizeof (Gate*) * specialGatesAmount);
	for (i = 0; i < arraySize; ++i) {
		if(GatesArray[i].flags != flagNone){
			sarray[count++] = &GatesArray[i];
		}
		if(count>specialGatesAmount){
			printf("Error: specialGatesCollector - count>specialGatesAmount");
			return NULL;
		}
	}
	return sarray;
}
