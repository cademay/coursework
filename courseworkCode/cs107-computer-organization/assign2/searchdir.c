/*
 * File: searchdir.c
 * Author: Cade May
 * ----------------------
 * This program recursively searches a directories to find and print various information, depending on flag used.
 * In addition to accepting designated search strings, it can also accept the following control flags: -i and -d.
 */

#include "cmap.h"
#include "cvector.h"
#include <dirent.h>
#include <error.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include <time.h>


#define NFILES_ESTIMATE 20
#define MAX_INODE_LEN   21  // digits in max unsigned long is 20, plus \0
#define DATE_MAX 6

/*
* this comparison function takes two void*, casts them to
*unsigned longs, and comparest them. it returns an integer
*/
int cmp_unsignedlong(const void *element1, const void *element2) 
{
	unsigned long num1 = *(unsigned long*)element1;
	unsigned long num2 = *(unsigned long*)element2;

	if (num1 < num2) return -1;
	if (num1 > num2 ) return 1;
	return 0;
}

/*
*this function takes a void*, casts it to a vector pointer, 
*then disposes of the vector that the pointer points to
*/
void destroyVectors(void *element) 
{
	CVector **vecPtr = element;
	cvec_dispose(*vecPtr);
}

/*
* this function takes a void*, casts it to a string pointer,
*then frees the char* that the string pointer points to by dereferncing it
*/
void freeCallBack(void *element)
{
	char **strPtr = element;
	free(*strPtr);
}

/*
*this method takes two void*s, casts them to strings,
*and then compares them by length. if the length is equal, it 
*compares them alphabetically, then returns an integer
*/
int cmp_str(const void *a, const void *b)
{
	char *strA = *(char**)a;
	char *strB = *(char**)b;

	if (strlen(strA) < strlen(strB)) return -1;
	if (strlen(strA) >  strlen(strB)) return 1;

	int tieBreaker = strcmp(strA,strB);
	if (tieBreaker < 0) return -1;
	if (tieBreaker > 0) return 1;

	return 0;
}

/*
*this method a takes a vector, two strings, and a visited vector. it recursively dives into the directory
*referred to by the string dirname and finds all the directories with it. if a file it finds contains the
*searchstr string, then it adds it to the matches vector that it receives, altering it remotelty
*/
static void gather_files(CVector *matches, const char *searchstr, const char *dirname, CVector *visited)
{
	DIR *dp = opendir(dirname); 
	struct dirent *entry;
	while (dp != NULL && (entry = readdir(dp)) != NULL) { /* iterate over entries in dir */
		if (entry->d_name[0] == '.') continue; /* skip hidden files */

		char fullpath[PATH_MAX];
		sprintf(fullpath, "%s/%s", dirname, entry->d_name); /* construct full path */

		struct stat ss;
		/* if subdirectory, recur */
		if (stat(fullpath, &ss) == 0 && S_ISDIR(ss.st_mode)) {

			/* inode number is unique id per entry in filesystem */
			unsigned long inode = ss.st_ino; 

			//(void)inode;
			if (cvec_search(visited, &inode, cmp_unsignedlong, 0, false) == -1) { 
				cvec_append(visited, &inode);
				gather_files(matches, searchstr, fullpath, visited);
			}

		} else {
			if (strstr(entry->d_name,searchstr) != NULL) {	
				char *heapPath = strdup(fullpath);
				cvec_append(matches, &heapPath);
			}
		}
	}
	closedir(dp);
}

/*
*this method receives a map, a directoryname string, and a visited vector. it recursively 
*delves into the directory that corresponds with the directoryname and finds all of the files
*it contains. it then adds each file it finds to the map, with the string representation of
*each file's inode as the key, and the file name for the value
*/
static void make_pairs(CMap *inodePairs, const char *dirname, CVector *visited)
{

	DIR *dp = opendir(dirname);
	struct dirent *entry;

	while (dp != NULL && (entry = readdir(dp)) != NULL) {
		if (entry->d_name[0] == '.') continue;

		char fullpath[PATH_MAX];
		sprintf(fullpath, "%s/%s", dirname, entry->d_name);

		struct stat ss;
		if (stat(fullpath, &ss) == 0 && S_ISDIR(ss.st_mode)) {

			unsigned long inode = ss.st_ino;

			if (cvec_search(visited, &inode, cmp_unsignedlong, 0, false) == -1) {
				cvec_append(visited, &inode);
				make_pairs(inodePairs, fullpath, visited);
			}
		}
		else {
			//turn inode into string, allocate memory for pathname, add inode:path to map
			char inodeString[MAX_INODE_LEN + 1];
			sprintf(inodeString, "%lu", ss.st_ino);
			char *heapPath = strdup(fullpath);
			cmap_put(inodePairs, inodeString, &heapPath);
		}
	}
	closedir(dp);
}

/*
*this method receives a map, a directoryname string, and a visited vector. this method
*recursively delves into the director receives and locates all of the files. for each file it
*finds, it locates the most recent modification date, and puts it the file into the map inside a vector of 
*other files with the same date. so the map has dates as keys, and the values are vectors containing
*files that correspond to that date
*/
static void sort_by_date(CMap *dateToFileMap, const char* dirname, CVector *visited)
{

	DIR *dp = opendir(dirname);
	struct dirent *entry;

	while (dp != NULL && (entry = readdir(dp)) != NULL) {
		if (entry->d_name[0] == '.') continue;

		char fullpath[PATH_MAX];
		sprintf(fullpath, "%s/%s", dirname, entry->d_name);

		struct stat ss;
		if (stat(fullpath, &ss) == 0 && S_ISDIR(ss.st_mode)) {

			unsigned long inode = ss.st_ino;

			if (cvec_search(visited, &inode, cmp_unsignedlong, 0, false) == -1) {
				cvec_append(visited, &inode);
				sort_by_date(dateToFileMap, fullpath, visited);
			}
		}
		else {

			struct stat ss2;
			if (stat(fullpath, &ss2) != 0) continue;
			struct tm *timeobj = localtime(&(ss2.st_mtime));
			char datebuf[DATE_MAX]; 
			strftime(datebuf, DATE_MAX, "%m/%d", timeobj);

			char *heapPath = strdup(fullpath);

			CVector **fileVecCheck = cmap_get(dateToFileMap, datebuf);			
			if (fileVecCheck == NULL) {
				//make new vector and add it to map with date as key
				CVector *fileVec = cvec_create(sizeof(char*),NFILES_ESTIMATE,freeCallBack);
				cvec_append(fileVec, &heapPath);
				cmap_put(dateToFileMap,datebuf,&fileVec);
			}
			else {
				cvec_append(*fileVecCheck, &heapPath);
				cmap_put(dateToFileMap,datebuf,fileVecCheck);
			}
		}
	}
	closedir(dp);
}



/*
*this method receives a directory name string. then it creates a map that takes vectors as values
*and then sends the dirname string and the map to another function that populates them map
*then, until the user enters 'q', this method asks the user to enter a MM/DD date combination
*and then this method checks to see if that combination corresponds to any vector value in the map
*and if it does it prints the contents of the vector (file names)
*/
static void dateSearch(const char *dirname)
{

	CMap *dateToFileMap = cmap_create(sizeof(CVector*), 10000, destroyVectors);
	CVector *visited =  cvec_create(sizeof(unsigned long*), 10000, NULL);

	sort_by_date(dateToFileMap, dirname, visited);

	while (true) {
		char userInput[200];
		printf("Enter date MM/DD (or q to quit): ");
		if (scanf("%s", userInput) >= 0) {
			if (strcmp(userInput, "q") == 0) break;
			CVector **fileVector = cmap_get(dateToFileMap, userInput);
			if (fileVector != NULL) {
				for (char **cur = cvec_first(*fileVector); cur != NULL; cur = cvec_next(*fileVector, cur)) {
					printf("%s\n", *cur);
				}
			}
		}
	}	
	cmap_dispose(dateToFileMap);
	cvec_dispose(visited);
}


/*
*this method creates a map that takes strings as values. then it calls another function
*that populates that map with respect to the dirname that this function receives.
*then as long as the user doesn't enter 'q', this function requests an inode number.
*when the user enters an inode number, it checks to see if that number is a valid key for the map
*if it is, it prints the corresponding string value. when the while loop breaks, it disposes of the map and the vector
*/
static void inodeSearch(const char *dirname)
{
	//MAKE MAP :   INODE VALUES:PATH NAMES
	CMap *inodePairs = cmap_create(sizeof(char*), 10000, freeCallBack);
	CVector *visited = cvec_create(sizeof(unsigned long*), 10000, NULL);

	//PASS MAP TO ANOTHER RECURSIVE SEARCH FUNCTION AND POPULATE
	make_pairs(inodePairs, dirname, visited);

	while (true) {
		char userInput[200];
		printf("Enter inode (or q to quit): ");
		if (scanf("%s",userInput) >= 0) {

			if (strcmp(userInput, "q") == 0) break;

			char **output = cmap_get(inodePairs, userInput);	
			if (output != NULL) {
				printf("%s\n",*output);
			}	
		}

	}

	cmap_dispose(inodePairs);
	cvec_dispose(visited);

}

/*
*this method creates a vector that take string. then it sends that vector, along with the searchstring
*and the directoryname it receives, along to another function that populates the map. then this function prints all
*the contents of the vector, which will contain all of the file names that corresponded to/contained the search string.
*then finally it disposes of both vector
*/
static void namesearch(const char *searchstr, const char *dirname)
{
	//CVector *matches = cvec_create(PATH_MAX, NFILES_ESTIMATE, NULL);
	CVector *matches = cvec_create(sizeof(char*), NFILES_ESTIMATE, freeCallBack);
	CVector *visited = cvec_create(sizeof(unsigned long*),10000,NULL);

	gather_files(matches, searchstr, dirname, visited);
	cvec_sort(matches,cmp_str);

	for (char **cur = cvec_first(matches); cur != NULL; cur = cvec_next(matches, cur))
		printf("%s\n", *cur);

	cvec_dispose(matches);
	cvec_dispose(visited);
}


/*
*this method decides which function to call based on the flag or lack thereof
*included or not included in the user's input
*/
int main(int argc, char *argv[])
{
	if (argc < 2) error(1,0, "Usage: searchdir [-d or -i or searchstr] [(optional) directory].");

	char *dirname = argc < 3 ? "." : argv[2];
	if (access(dirname, R_OK) == -1) error(1,0, "cannot access path \"%s\"", dirname);

	if (0 == strcmp("-d", argv[1])) {
		dateSearch(dirname);
	} else if (0 == strcmp("-i", argv[1])) {
		inodeSearch(dirname);
	} else {
		// search by name
		namesearch(argv[1], dirname);
	}
	return 0;
}

