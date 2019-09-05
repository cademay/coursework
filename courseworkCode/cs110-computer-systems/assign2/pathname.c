/*
* Author: Cade May
* Class: CS110 Spring
* Date: April 17, 2017
* pathname.c
* This file implements the pathname_lookup method
*/
#include "pathname.h"
#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/**
 * Returns the inode number associated with the specified pathname.  This need only
 * handle absolute paths.  Returns a negative number (-1 is fine) if an error is 
 * encountered.
 */
int pathname_lookup(struct unixfilesystem *fs, const char *pathname) {
	
	if (pathname[0] != '/') return -1;
	if (strlen(pathname) == 1) return ROOT_INUMBER;	
	
	int pathLength = strlen(pathname);
	char pathCopy[pathLength];
	
	memcpy(pathCopy, pathname + 1, pathLength);
	char *path = pathCopy;	
	
	char *pathSegment;
	int curDirinumber = ROOT_INUMBER;
	
	while ((pathSegment = strsep(&path, "/")) != NULL) {
		struct direntv6 curDirent;
		if (directory_findname(fs, pathSegment, curDirinumber, &curDirent) != 0) return -1;
		curDirinumber = curDirent.d_inumber; 	
	}
	
	return curDirinumber;
}
