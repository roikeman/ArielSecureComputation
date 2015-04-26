/*
 * secCompMultiParty.h
 *
 *  Created on: Mar 31, 2015
 *      Author: froike
 */



#ifndef SECCOMPMULTIPARTY_H_
#define SECCOMPMULTIPARTY_H_

/*
 * define section (pre-compilation values)
 * STRING_BUFFER_SIZE will describes the maximum buffer length.
 */

#define STRING_BUFFER_SIZE 256
#define true 1
#define false 0
#define flagXor 1
#define flagXnor 2
#define flagNone 0
#define dispXor "Xor"
#define dispXnor "Xnor"
#define dispNone "None"


/*
 * This struct present one player and array of his inputs bit serials.
 */
typedef struct player{
	unsigned int playerBitAmount;
	unsigned int * playerBitArray;

} Player;

/*
 * This struct present one gate with two inputs bit, one output bit and a Truth table.
 *
 */
typedef struct gate{
	unsigned int inputBit1;
	unsigned int inputBit2;
	unsigned int outputBit;
	unsigned int truthTable; // : 4;
	unsigned int flags : 2; //limited for 2 bits
} Gate;

typedef struct cycle{
	unsigned int amountOfPlayers;
	unsigned int amountOfGates;
	Gate  ** specialGates;
	Player outputbits;
	Gate * gateArray;
	Player ** playerArray;
} Cycle;

/*
 * This function got path fo file that present an circle (format instructions below).
 * if it success to create circle then This function return an Circle struct.
 * else it will return NULL pointer and print the reason for failing.
 *
 * Circle file format will start with "<amount of Gates> <amount of Players>" as to numbers split with one space.
 *
 * Next lines will contains the player ID and the amount of input bits he need. after that, line by line, one line for each bit serial.
 * Exemple:
 * P<player ID> <amount of player inputs bit>
 * <first input bit serial>
 * <second input bit serial>
 * 			.
 * 			.
 *
 * 	in this way it collect player after player until it collect many players as described in <amount of Players>...
 *
 * 	then will need to come the gates describes lines. each line for gate in this format:
 * 	<first input bit> <sec input bit> <output bit> <truth Table> <flags>
 * 	the truth Table will be in format of 4 digit length contains just 1 or 0 digits.
 * 	flags is optional.
 *
 * 	Exmple for valid file:
 * 		# if you have notes you can write it start with '#'... and we just ignore that lines.
 * 	4 3
 *		# Players:
 *	P0 3
 *	0
 *	1
 *	2
 *	P1 3
 *	3
 *	4
 *	5
 *	P2 3
 *	6
 *	7
 *	8
 *		# Gates:
 *	0 1 9 1010
 *	2 3 10 1101 freeXor
 *	4 5 11 0010
 *	6 7 12 0101
 *
 */
extern Cycle * readCycleFromFile(char path[]);

/*
 * private function that crate gates structs. it used in readCycleFromFile function.
 */
Gate GateCreator(const unsigned int inputBit1, const unsigned int inputBit2, const unsigned int outputBit, const unsigned int TTable, const unsigned int flags);

/*
 * this function print full given Cycle it cycle file format.
 * (the format explanation can be found in this h file at readCycleFromFile function documentation.)
 */
extern void printCycle(const Cycle * C);

/*
 * this function remove all spaces from the string it get.
 */
void removeSpacesAndTubs(char* source);

/*
 * This function react as destroyer for Circles. it will free all the dynamic memory it use.
 * Attention: if you forget use this function after done with circle, parts of it will remain in the memory until your program will terminated.
 */
void freeCircle(Cycle * c);

Gate ** specialGatesCollector(Gate * GatesArray, const unsigned int arraySize, const unsigned int specialAmount);

#endif /* SECCOMPMULTIPARTY_H_ */
