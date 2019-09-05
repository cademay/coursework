/*
 * File: cmap.c
 * Author: Cade May
 * ----------------------
 * This program details the inner workings of the CMap type.
 */

#include "cmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <stdbool.h>
// a suggested value to use when given capacity_hint is 0
#define DEFAULT_CAPACITY 1023

/* Type: struct CMapImplementation
 * -------------------------------
 * This definition completes the CMap type that was declared in
 * cmap.h. You fill in the struct with your chosen fields.
 */
struct CMapImplementation {

	void **buckets;
	int nbuckets;
	int valsz;
	int curCount;
	CleanupValueFn clean;
};


/* The NOT_YET_IMPLEMENTED macro is used as the body for all functions
 * to remind you about which operations you haven't yet implemented.
 * It wil report an error if a call is made to an not-yet-implemented
 * function (this is preferable to returning garbage or silently
 * ignoring the call).  Remove the call to this macro as you implement
 * each function and finally you can remove this macro and comment
 * when no longer needed.
 */
#define NOT_YET_IMPLEMENTED printf("%s() not yet implemented!\n", __func__); raise(SIGKILL); exit(107);



/* Function: hash
 * --------------
 * This function adapted from Eric Roberts' _The Art and Science of C_
 * It takes a string and uses it to derive a "hash code," which
 * is an integer in the range [0..nbuckets-1]. The hash code is computed
 * using a method called "linear congruence." A similar function using this
 * method is described on page 144 of Kernighan and Ritchie. The choice of
 * the value for the multiplier can have a significant effort on the
 * performance of the algorithm, but not on its correctness.
 * The computed hash value is stable, e.g. passing the same string and
 * nbuckets to function again will always return the same code.
 * The hash is case-sensitive, "ZELENSKI" and "Zelenski" are
 * not guaranteed to hash to same code.
 */
static int hash(const char *s, int nbuckets)
{
	const unsigned long MULTIPLIER = 2630849305L; // magic number
	unsigned long hashcode = 0;
	for (int i = 0; s[i] != '\0'; i++)
		hashcode = hashcode * MULTIPLIER + s[i];
	return hashcode % nbuckets;
}

//this method returns the next blob in the linked list
//with respect to the blob it receives
static void *getNextBlob(void *blob) {
	void *toReturn = *(void**)blob;
	return toReturn;

}

//this method returns the key associated with the data
//blob it receives
static char *getKey(void *blob) {
	return (char*)blob + sizeof(void*);
}

//this method gets the value from the value position
//of the data blob it receives
static void *getValue(void *blob, CMap *cm) {

	int start = 8;
	int i = 0;

	while (1) {

		char *cur = (char*)blob + start + i;
		char curChar = cur[0];
		if (curChar == '\0') break;
		i++;
	}

	int nullTerminatorSpot = 8 + i;
	return (char*)blob + nullTerminatorSpot + 1;
}

//this method sets the value position of the data blob it receives
//using the value data it receives
static void setValue(void *blob, CMap *cm, const void *addr) {

	int start = 8;
	int i = 0;
	while (1) {
		char *cur = (char*)blob + start + i;
		char curChar = cur[0];
		if (curChar == '\0') break;
		i++;
	}

	int nullTerminatorSpot = 8 + i;
	memcpy( (char*)blob + nullTerminatorSpot + 1, addr, cm->valsz);
}

//this method creates a new cmap struct and places it on the heap
CMap *cmap_create(size_t valuesz, size_t capacity_hint, CleanupValueFn fn)
{
	void **bucketsPtr;
	bucketsPtr = calloc(capacity_hint, sizeof(void*));
	
	int roomForCleanup = 6;
	struct CMapImplementation *heapMap;
	heapMap = malloc( sizeof(void**) + 3*sizeof(int) + sizeof(void*) + roomForCleanup );

	(*heapMap).buckets = bucketsPtr;
	(*heapMap).nbuckets = capacity_hint;
	(*heapMap).valsz = valuesz;
	(*heapMap).curCount = 0;
	(*heapMap).clean = fn;
	return heapMap;
}

//this method frees the memory associated with the cmap it receives
//if a cleanup function is provided, it calls it on all the values before
//disposing of all the memory.
void cmap_dispose(CMap *cm)
{
	for (int i = 0; i < cm->nbuckets; i++) {
		void *head = cm->buckets[i];
		void *curr = head;

		while (curr != NULL) {

			void *temp = curr;
			curr = getNextBlob(curr);

			if (cm->clean != NULL) {
				cm->clean(getValue(temp, cm));
			}
			free(temp);
		}
	}	

	free(cm->buckets);
	free(cm);
}

//this method returns the current amount of items
//in the map it receives
int cmap_count(const CMap *cm)
{
	return cm->curCount;
}

//this method takes a cmap, a key, and an address. it places a data blob
//into a linked list with a null pointer as its next value
static void put_blob(CMap *cm, const char *key, const void *addr) {
	int hashVal = hash(key, cm->nbuckets);
	void *newBlob;
	int blobSize = sizeof(void*) + strlen(key) + 1 + cm->valsz;
	newBlob = malloc(blobSize);
	void *nullPtr = NULL;
	memcpy( (char*)newBlob + 0, &nullPtr, sizeof(void*) );
	strcpy( (char*)newBlob + sizeof(void*), key );
	memcpy((char*)newBlob + sizeof(void*) + strlen(key) + 1, addr, cm->valsz);
	cm->buckets[hashVal] = newBlob;
	cm->curCount++;
}

//this method takes a cmap, a key, an address, and a pointer to a previous data blob
//it redirects the pointers so that the linked list is rearranged
//it returns a newblob that pointer to the old blob, and contains the new values provided
//this newblob is then placed at the front of the linked list in the calling function
static void *link_new_blob(CMap *cm, const char *key, const void *addr, void **oldPointer) {

	void *newBlob;
	int blobSize = sizeof(void*) + strlen(key) + 1 + cm->valsz;
	newBlob = malloc(blobSize);
	memcpy( (char*)newBlob, &*oldPointer, sizeof(void*) );
	strcpy( (char*)newBlob + sizeof(void*), key );
	memcpy((char*)newBlob + sizeof(void*) + strlen(key) + 1, addr, cm->valsz);

	return newBlob;	
}

//this method takes a cmap, a key, and a values, and associates them in a data blob,
//and then places that blob into the map. if there are no other keys in the map with
//the same hash value, then this method just simply adds the data blob to the map.
//if there is another blob in the corresponding hash value bucket,then it performs
//linked list operations in order to connect the blobs in the bucket in an easily 
//accessible way//
void cmap_put(CMap *cm, const char *key, const void *addr)
{	
	int hashVal = hash(key, cm->nbuckets);
	void *firstBlob = cm->buckets[hashVal];
	bool set = false;
	if (firstBlob != NULL) {
		while (firstBlob != NULL) {

			char *keyInLinkedList = getKey(firstBlob);		
			if (strcmp(key, keyInLinkedList) == 0) {
				if (cm->clean != NULL) {
					void *value = getValue(firstBlob, cm);
					cm->clean(value);
				}
				setValue(firstBlob, cm, addr);
				set = true;
			}
			firstBlob = getNextBlob(firstBlob);

		}

		//set becomes true if the given key already existed in the map
		//in this case, the value is simply replaced (see above)
		//if there is an overlap in hash values, but it's not the same string,
		//then we reach this case below, which involves placing a new data blob
		//at the very front of the linked list
		if (!set) {
			void *oldPointer = cm->buckets[hashVal];
			void *newBlob = link_new_blob(cm, key, addr, &oldPointer);
			cm->buckets[hashVal] = newBlob;
			cm->curCount++;
		}

	} else {
		put_blob(cm, key, addr);
	}	
}

//this method takes a cmap and a key and returns the value associated with
//the key. it does this by identifying which hash bucket the key should fall into
//and the searching each of the nodes in that linked list. if nothing is found,
//it returns null
void *cmap_get(const CMap *cm, const char *key)
{
	int hashVal = 0;
	hashVal = hash(key, cm->nbuckets);
	void *selectedBucket = cm->buckets[hashVal];

	while (selectedBucket != NULL) {
		char *bucketKey = getKey(selectedBucket);
		if (strcmp(bucketKey, key) == 0) {
			int valueStart = 8 + strlen(key) + 1;
			return (char*)selectedBucket + valueStart;
		}
		selectedBucket = getNextBlob(selectedBucket);
	}
	return NULL;
}

//this method takes a string and a cmap and uses the string to
//determine which bucket the string came from with respect to the map
//it does this by iterating over the nodes in each bucket and searching 
//for a matching key until it finds a match. else it returns NULL
void *whichBucket(const CMap *cm, const char *key)
{
	bool found = false;
	int hashVal = 0;
	hashVal = hash(key, cm->nbuckets);
	void *selectedBucket = cm->buckets[hashVal];

	while (selectedBucket != NULL) {
		char *bucketKey = getKey(selectedBucket);
		if (strcmp(bucketKey, key) == 0) {
			found = true;
			break;
		}
		selectedBucket = getNextBlob(selectedBucket);
	}

	if (found) {       
		return selectedBucket;
	} else {
		return NULL;
	}
}

void cmap_remove(CMap *cm, const char *key)
{ NOT_YET_IMPLEMENTED }

//this method receives a CMap and return a pointer to its first value
const char *cmap_first(const CMap *cm)
{
	for (int i = 0; i < cm->nbuckets; i++) {
		void *curBucket = cm->buckets[i];	
		if (curBucket != NULL) return getKey(curBucket);
	}
	return NULL;
}

//this method receives a previous key and and a cmap.
//it first calls a method to find which blob the key came from
//then it finds the next blob using the getnextblob method
//if next is null, then it will iterate over all of the buckets
//of hash values beyond curBlob and next and find the next blob 
//through iteration. returns null if there are no more map items
const char *cmap_next(const CMap *cm, const char *prevkey)
{
	void *curBlob = whichBucket(cm, prevkey);

	void *next = getNextBlob(curBlob);

	if (next == NULL) {

		int hashVal = 0;
		hashVal = hash(prevkey, cm->nbuckets);

		for (int i = hashVal + 1; i < cm->nbuckets; i++) {
			void *curBucket = cm->buckets[i];
			if (curBucket != NULL) return getKey(curBucket);
		}


	} else {
		return getKey(next);
	}

	return NULL;
}
