/*
* Author: Cade May
* Class: CS110 Spring
* Date: April 17, 2017
* File.c
* This file implements the file_getblock method
*/

#include <stdio.h>
#include <assert.h>

#include "file.h"
#include "inode.h"
#include "diskimg.h"

/**
 * Fetches the specified file block from the specified inode.
 * Returns the number of valid bytes in the block, -1 on error.
 */
int file_getblock(struct unixfilesystem *fs, int inumber, int blockNum, void *buf) {
	
	struct inode ino;
	if (inode_iget(fs, inumber, &ino) != 0) return -1;
	
	int index = inode_indexlookup(fs, &ino, blockNum);
	if (index == -1) return -1;
	
	int inoSize = inode_getsize(&ino);
	int numFullBlocks = inoSize / DISKIMG_SECTOR_SIZE;	
	int numBytes = diskimg_readsector(fs->dfd, index, buf);
	
	if (blockNum < numFullBlocks) return DISKIMG_SECTOR_SIZE;
	else return inoSize % DISKIMG_SECTOR_SIZE;

}
