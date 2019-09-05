/*
 * File: bits.c
 * Author: Cade May
 * ----------------------
 * This program jumps through a variety of sanity check hoops by implementing a function
 * that compares bits, two Sudoku helper functions (one of which makes a set, and one
 * which determines if there is only one move left), two general number overflow methods,
 * and a disassembly method.
 */

#include "bits.h"
#include <assert.h>
#include <math.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
/* The NOT_YET_IMPLEMENTED macro is used as the body for all functions
 * to remind you about which operations you haven't yet implemented.
 * It wil report an error if a call is made to an not-yet-implemented
 * function (this is preferable to returning garbage or silently
 * ignoring the call).  Remove the call to this macro as you implement
 * each function and finally you can remove this macro and comment
 * when no longer needed.
 */
#define NOT_YET_IMPLEMENTED printf("%s() not yet implemented!\n", __func__); raise(SIGKILL); exit(107);

//this method takes two integers and returns a negative number if
//the first has fewer bits "on" than the second, and a positive number if the first 
//has more bits "on" than the second
int cmp_bits(int a, int b)
{
	int aCount = 0;
	int bCount = 0;
	int mask = 0;

	for (int i = 0; i < 32; i++) {
		mask = 1 << i;
		if ((a & mask) != 0) aCount++;
		if ((b & mask) != 0) bCount++; 
	}
	return aCount - bCount;
}

//this method takes in an int array and an int corresponding to the array size
//it returns a bitstring that represents the values in the given set it receives.
//The on or off status of bits in positions 1-9 of the returned result mark the 
//set membership of the corresponding 1-9 integer, and the remaining bits are zeros
unsigned short make_set(int values[], int nvalues)
{	
	int bitVectorSet = 0;
	int mask = 0;

	for (int i = 0; i < nvalues; i++) {
		int cur = values[i];
		mask = 1 << cur;
		bitVectorSet = bitVectorSet | mask;			
	}	

	return bitVectorSet;
}

//this method receives 3 unsigned short bitstrings and uses bit manipulation to
//determine if there is only one remaining numerical option that is not accounted
//for in the given bitstrings in the positions 1-9, starting at the LSB.
//this method returns true if there is only one possible number 1-9 that is
//not already accounted for by the 3 given bitstrings
bool is_single(unsigned short used_in_row, unsigned short used_in_col, unsigned short used_in_block)
{
	int combined = ( used_in_row | used_in_col ) | used_in_block;
	int combinedInverse = ~combined;
	int clearLeft = ((1 << 10) - 1) & combinedInverse;
	int combinedInverseWithoutLSB = clearLeft >> 1;
	if (((combinedInverseWithoutLSB & (-combinedInverseWithoutLSB)) == combinedInverseWithoutLSB) && combinedInverseWithoutLSB != 0) return true;
	return false;
}

//this method returns the sum of two given numbers if their sum value
//doesn't overflow the representable range given by the amount of 
//bits they're contained if. if their sum does overflow, it returns
//the maximum value of the given number of bits, rather than returning
// an indeterminate overflow value
utype sat_add_unsigned(utype a, utype b)
{
	utype zero = 0ULL;
	utype val = a + b;
	utype zeros = a & zero;
	utype ones = ~zeros;
	utype MSBmask = ~(ones >> 1);
	
	//checks to see if both most significant bits are turned on
	//if both are, then this number will overflow, so return 1111.....11111, the unsigned max
	if (((a & MSBmask) != 0) && ((b & MSBmask) != 0)) return ones;
	return val;
}

//this method returns the sum of two given numbers if their value
//doesn't overflow or underflow the representable range given by the amount of 
//bits they're contained if. if their sum does overflow, it returns
//the maximum value or minimum value of the given number of bits, rather than returning
//an indeterminate overflow or underflow value
stype sat_add_signed(stype a, stype b)
{
	utype zero = 0ULL;
	utype zeros = a & zero;
	utype ones = ~zeros;
	utype maxVal = (ones >> 1);
	utype overflowMask = (( (~(ones >> 1)) >> 1)) & maxVal;
	utype signBitMask = ~maxVal;
	utype underflowMask = overflowMask;
	utype minVal = ~(ones >> 1);

	//if both negative
	if (( (a & signBitMask) !=  0) && ( (b & signBitMask) != 0)) {
		//if both too negative
		if (( (a & underflowMask) == 0) && ( (b & underflowMask) == 0)) return minVal;

	}	

	//if both positive
	if (( (a & signBitMask) == 0) && ( (b & signBitMask) == 0)) {
		// if both too positive;
		if (( (a & overflowMask) != 0) && ( (b & overflowMask) != 0)) return maxVal;
	}

	return a + b; 
}

#define MAX_BYTES 5

/* Function: print_hex_bytes
 * --------------------------
 * Given the pointer to a sequence of bytes and a count, and this
 * function will print count number of raw bytes in hex starting at
 * the bytes address, filling in with spaces to align at the width
 * of the maximum number of bytes printed (5 for push).
 */
void print_hex_bytes(const unsigned char *bytes, int nbytes)
{
	for (int i = 0; i < MAX_BYTES; i++)
		if (i < nbytes)
			printf("%02x ", bytes[i]);
		else
			printf("  ");
	printf("\t");
}

// Each register name occupies index of array according to number
const char *regnames[] = {"%rax", "%rcx", "%rdx", "%rbx", "%rsp", "%rbp", "%rsi", "%rdi"};

//this method receives an array of unsigned chars representing x86 machine code, interprets the 
//given values, and prints out the the human-readable assembly code equivalent.
//it does this by using a series of bit manipulations and checks in order to determine the 
//values of the machine instructions
void disassemble(const unsigned char *raw_instr)
{
	unsigned char case1 = 0x68;	
	unsigned char firstByte = raw_instr[0];
	unsigned char registerMask = 0x58;
	unsigned char registerCheck = 0x50;
	unsigned char registerIdentifier = 0x07;
	unsigned char secondByte;
	unsigned char ones = ~0;
	unsigned char indirectMask = 0xF8;
	unsigned char indirectCheck = 0x30;
	unsigned char indirectDisplacementCheck = 0x70;
	unsigned char thirdByte = 0x00;
	unsigned char scaleCheck = 0x03;
	unsigned char displacementMask = 0x0F;
	const char *reg1, *reg2;
	const int scaleValues[] = {1, 2, 4, 8};
	int registerNumber, displacement1, displacement2, scalePlace, scale, disp;

	//immediate constant case	
	if (firstByte == case1) {
		print_hex_bytes(raw_instr, 5);
		printf("pushq $0x");
		printf("%02x%02x\n", raw_instr[2], raw_instr[1]);
	}
	
	//register case
	if ((firstByte & registerMask) == registerCheck) {
		registerNumber = firstByte & registerIdentifier;
		print_hex_bytes(raw_instr, 1);
		printf("pushq %s\n", regnames[registerNumber]);
	} 

	secondByte = raw_instr[1];
	if ((firstByte & ones) == ones) {
		secondByte = raw_instr[1];		
		registerNumber = secondByte & registerIdentifier;
		
		//indirect with displacement and scaled-index case	
		if ((registerNumber == 4) || registerNumber == 5) {
			thirdByte = raw_instr[2];
			reg1 = regnames[thirdByte & registerIdentifier];
			reg2 = regnames[((thirdByte >> 3) & registerIdentifier)];
			disp = raw_instr[3];
			scalePlace = (thirdByte >> 6) & scaleCheck;			
			scale = scaleValues[scalePlace];
			print_hex_bytes(raw_instr, 4);
			printf("pushq 0x%02x(%s,%s,%d)\n", disp, reg1, reg2, scale);
			secondByte = 0xFF; //cancelling other if statements
		}	
		
		//indirect case
		if ((secondByte & indirectMask) == indirectCheck) {
			print_hex_bytes(raw_instr, 2);
			printf("pushq (%s)\n", regnames[registerNumber]);
		}

		//indirect with displacement case
		if ((secondByte & indirectMask) == indirectDisplacementCheck) {
			thirdByte = raw_instr[2];			
			displacement1 = thirdByte & displacementMask;
			displacement2 = ( (thirdByte >> 4) & displacementMask);
			print_hex_bytes(raw_instr, 3);

			if (displacement2 == 0) {
				printf("pushq 0x%d(%s)\n", displacement1, regnames[registerNumber]);
			} else {
				printf("pushq 0x%d%d(%s)\n", displacement2, displacement1, regnames[registerNumber]);
			}
		}
	}
}



