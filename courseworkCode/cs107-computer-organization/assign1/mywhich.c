/*
 * File: mywhich.c
 * Student: Cade May
 * -----------------------
 *
 * The 'mywhich' program implements a simplified version of the 'which' command.
 * It is used to search for commands by name. It accepts one or more aguments
 * (each the name of a command) and searchs for matching executable in each of the 
 * directories in the PATH environment variable. It prints the full path of the
 * first matching exectuable found. The optional -p flag allows user to specify
 * the search path which is used instead of reading PATH from the environment.
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>

void getPathName(char *envp[], char **pathName);
int parsePath(char pathArray[200][PATH_MAX], char *pathName);

/* Function: main
 * --------------
 * main ends the program immediately if the int argc it receives is < 1
 * otherwise it checks for the -p flag, and sends the path name to the provided string if -p is present
 *if -p is not prsent, it calls a method to set the standard path name.
 *then it calls a method to parse the path name into an array of path names
 *then for each element of argv it checks to see if any of the path names
 *can be combined with it to create a valid file path. if so, it prints it.
 * Ordinarily main() has just two arguments (argc/argv); this alternate
 * prototype includes an optional third argument which is an array of
 * environment variables. Each array entry is of a string of the form
 * "NAME=value". The array has NULL after the last entry as a sentinel 
 * to identify the end of array. 
 * Programs can use either form for main. The 2-argument version is more 
 * common by far, but "mywhich" uses the 3-argument version to access 
 * the environment variables.
 */
int main(int argc, char *argv[], char *envp[]) {

	if (argc > 1) {
		char *pathName = NULL;
		if (strcmp(argv[1],"-p") != 0) {
			getPathName(envp,&pathName);
			char eq = '=';
			pathName = strchr(pathName,eq) + 1;
		} 
		else {
			pathName = argv[2];
		}
		//make array of possible paths
		char pathArray[200][PATH_MAX];
		int numPaths = parsePath(pathArray,pathName);

		for (int i = 1; i < argc; i++) {
			char *currentCommand = argv[i];
			for (int j = 0; j < numPaths; j++) {
				char tempPath[PATH_MAX];
				sprintf(tempPath, "%s%s%s",pathArray[j],"/",currentCommand);
				if (access(tempPath,X_OK) == 0)	printf("%s\n",tempPath);
			}
		}
	}
	return 0;
}

/*
 *This method receives a 2D array that it populates remotely, as well a pathName to parse
 *this method finds the location of each colon in the path, and adds the string segment into 
 *the path array. it stops when there are no more colons, breaks out of the loop, and returns
 *the amount of segments it created
 */
int parsePath(char pathArray[200][PATH_MAX], char *pathName) {

	char colon = ':';
	char *currentPath = pathName;
	char *colonLocation;
	int pathSegmentLength = 0;
	bool end = false;
	int numSegments = 0;

	while (end == false) {
		char pathSegment[100];
		colonLocation = strchr(currentPath,colon);
		memset(pathSegment, '\0',strlen(pathSegment));

		if (colonLocation == NULL) {
			strcpy(pathSegment,currentPath);
			end = true;
		}
		else {
			pathSegmentLength = colonLocation - currentPath;
			strncpy(pathSegment,currentPath,pathSegmentLength);
			currentPath = colonLocation + 1;	
		}
		strcpy(pathArray[numSegments],pathSegment);
		numSegments++;
	}
	return numSegments;
}

/*
 *This method receives a string array of environment variables, and a pointer to
 *a string, which it modifies remotely. this method iterates over the env variables
 *and when it finds one that begins with path, it dereferences the pointer to the string
 *and then breaks and concludes
 */
void getPathName(char *envp[], char **pathName) {

	for(int i = 0; envp[i] != NULL; i++) {
		char *currentStr = envp[i];
		if (strncmp(currentStr,"PATH",4) == 0) {
			*pathName = currentStr;
			break;
		}
	}
}

