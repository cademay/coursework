/*
* Author: Cade May
* Class: CS110 Spring
* Date: April 17, 2017
* Directory.c
* This file implements the directory_findname method
*/

#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include "file.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/**
 * Looks up the specified name (name) in the specified directory (dirinumber).  
 * If found, return the directory entry in space addressed by dirEnt.  Returns 0 
 * on success and something negative on failure. 
 */
int directory_findname(struct unixfilesystem *fs, const char *name,
		       int dirinumber, struct direntv6 *dirEnt) {
  	
	struct inode ino;
	if (inode_iget(fs, dirinumber, &ino) != 0) return -1; 
	
	//if not a directory
	if ( (ino.i_mode & IFMT) != IFDIR) return -1;
	
	int direntsPerBlock = DISKIMG_SECTOR_SIZE / sizeof(struct direntv6);	
	struct direntv6 direntArray[direntsPerBlock];

	int inodeSize = inode_getsize(&ino);
	int numBlocks = (inodeSize / DISKIMG_SECTOR_SIZE) + 1;	

	for (int i = 0; i < numBlocks; i++) {

		int numBytes = file_getblock(fs, dirinumber, i, direntArray);
		int numDirents = numBytes / sizeof(struct direntv6);
		for (int j = 0; j < numDirents; j++) {
			char *curName = direntArray[j].d_name;
			if ( strncmp(curName, name, 14) == 0) {
				*dirEnt = direntArray[j];
				return 0;
			}
		}
	}

	return -1;
}
