/*
 * File: cvector.c
 * Author: Cade May
 * ----------------------
 * This program implements the CVector interface
 */

#include "cvector.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <assert.h>
// a suggested value to use when given capacity_hint is 0
#define DEFAULT_CAPACITY 16

/* Type: struct CVectorImplementation
 * ----------------------------------
 * This struct defines a CVector object. 
 */
struct CVectorImplementation {
	int sizeOfElement;
	int capacityMax;
	void *listPtr;
	int curTotal;
	CleanupElemFn clean;
};


/* The NOT_YET_IMPLEMENTED macro is used as the body for all functions
 * to remind you about which operations you haven't yet implemented.
 * It will report a fatal error if a call is made to an not-yet-implemented
 * function (this is preferable to returning garbage or silently
 * ignoring the call).  Remove the call to this macro as you implement
 * each function and finally you can remove this macro and comment
 * when no longer needed.
 */
#define NOT_YET_IMPLEMENTED printf("%s() not yet implemented!\n", __func__); raise(SIGKILL); exit(107);

//This method creates a new CVector struct and places it on the heap.
//It takes size and capacity ints, and an optional cleanup function
CVector *cvec_create(size_t elemsz, size_t capacity_hint, CleanupElemFn fn)
{
	void *list;
	list = malloc(capacity_hint*(elemsz));
	int cleanupSize = 14;
	struct CVectorImplementation *heapVec;
	heapVec = malloc( sizeof(void*) + 3*(sizeof(int)) + cleanupSize );

	(*heapVec).sizeOfElement = elemsz;
	(*heapVec).capacityMax = capacity_hint;
	(*heapVec).listPtr = list;
	(*heapVec).curTotal = 0;
	(*heapVec).clean = fn;

	return heapVec;
}

//this method takes a CVector and frees all the memory allocated for it
//if the cvector struct came with a cleanup function, it is called on all its elements here
void cvec_dispose(CVector *cv)
{
	int count = cv->curTotal;
	int elemSize = cv->sizeOfElement;

	if (cv->clean != NULL) {
		for (int i = 0; i >=  count; i++) {
			cv->clean( (char*)(cv->listPtr) + i*elemSize );	
		}
	}

	free(cv->listPtr);
	free(cv);
}

//this method takes a cvector and returns the number of elements it holds
int cvec_count(const CVector *cv)
{
	return cv->curTotal;
}

//this method returns the (index)th element, depending 
//the index integer passed by the user
void *cvec_nth(const CVector *cv, int index)
{
	assert( index >= 0 );
	assert( index < cv->curTotal );
	int elemSize = (*cv).sizeOfElement;
	int spot = index * elemSize;

	void *list = (*cv).listPtr;
	return (char*)list + spot;		
}

//this method takes a cvector struct, an element, and an index
//it attempts to place the element at that index by shifting up
//the higher elements to make room
void cvec_insert(CVector *cv, const void *addr, int index)
{
	assert(index >= 0);
        assert(index <= cv->curTotal);
	
	int count = cv->curTotal;
	int elemSize = cv->sizeOfElement;

	void *list = cv->listPtr;
	for (int i = count - 1; i >=  index; i--) {

		void *cur = (char*)list + i*elemSize;
		void *moveSpot = (char*)list + (i + 1)*elemSize;
		memcpy(moveSpot,cur,elemSize);
	}


	memcpy((char*)list + index*elemSize,addr,elemSize);
	(*cv).listPtr = list;
	(*cv).curTotal = count + 1;

	if (cv->curTotal == cv->capacityMax) {

		cv->listPtr = realloc(cv->listPtr, 2 * cv->capacityMax * cv->sizeOfElement);
		cv->capacityMax = cv->capacityMax * 2;

	}
}

//this method places a new element on the end of the cvector
void cvec_append(CVector *cv, const void *addr)
{
	int elemsz = cv->sizeOfElement;
	int curCount = cv->curTotal;

	int newSpot = curCount * elemsz;

	memcpy((char*)cv->listPtr + newSpot,addr,elemsz);

	(*cv).curTotal = curCount + 1;

	if (cv->curTotal == cv->capacityMax) {
		cv->listPtr = realloc(cv->listPtr, 2 * cv->capacityMax * cv->sizeOfElement);
		cv->capacityMax = cv->capacityMax * 2; 
	}
}

void cvec_replace(CVector *cv, const void *addr, int index)
{ NOT_YET_IMPLEMENTED }

void cvec_remove(CVector *cv, int index)
{ NOT_YET_IMPLEMENTED }

//this method searches the cvector it receives for an element that matches the provided key
//it returns -1 if nothing is found. if it finds a match then it returns the first instance of //the match
int cvec_search(const CVector *cv, const void *key, CompareFn cmp, int start, bool sorted)
{
	assert(start >= 0);
	assert(start <= cv->capacityMax);

	void *list = cv->listPtr;

	if (sorted == false) {
		//linear search
		int elemsz = cv->sizeOfElement;
		for (int i = start; i < cv->curTotal; i++) {

			void *cur = (char*)list + (i*elemsz);
			if ( cmp(cur, key) == 0) {
				return i;
			}
		}

	} else {

		//binary search
		int beginning = 0;
		int end = cv->curTotal;
		int middle = (beginning + end)/2;
		int comparisonValue;
		while (true) {

			void *middleElem = (char*)list + middle*(cv->sizeOfElement);

			comparisonValue = cmp(middleElem,key);
			if (comparisonValue == 0) return middle;

			if (comparisonValue < 0) {
				beginning = middle + 1;
				middle = (beginning + end)/2;
				if (middle == end) break;
			}	

			if (comparisonValue > 0) {
				end = middle - 1;
				middle = (beginning + end)/2;
				if (end < 0) break;
			}
		}
	}

	return -1;
}

//this method calls qsort to sort the array of the cvector it receives
void cvec_sort(CVector *cv, CompareFn cmp)
{
	qsort(cv->listPtr, cv->curTotal, cv->sizeOfElement, cmp);
}

//this method returns the first element of the cvector it receives
//by accessing the cvector's array
void *cvec_first(const CVector *cv)
{
	if ((*cv).curTotal > 0) {
		return (*cv).listPtr;
	}
	else {
		return NULL;
	}
}

//this method receives a cvector and a previous element 
//and uses it to find the next element in the vector's array
void *cvec_next(const CVector *cv, const void *prev)
{
	void *start = (*cv).listPtr;

	int oldSpot = (char*)prev - (char*)start;
	int elemSize = (*cv).sizeOfElement;

	int newSpot = oldSpot + elemSize;
	if (newSpot / elemSize < (*cv).curTotal) {
		return (char*)start + newSpot;
	}
	else {
		return NULL;
	}
}
