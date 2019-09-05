/*
* Author: Cade May
* Class: CS110 Spring
* Date: April 17, 2017
* inode.c
* This file implements the methods inode_iget, inode_indexlookup, and inode_getsize
*/

#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

#include "inode.h"
#include "diskimg.h"

/**
 * Fetches the specified inode from the filesystem. 
 * Returns 0 on success, -1 on error.  
 */
int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp) {
	
	if (inumber < 1) return -1;
	inumber -= ROOT_INUMBER;	
	int fileDescriptor = fs->dfd;

	int inodesPerBlock = DISKIMG_SECTOR_SIZE / sizeof(struct inode);
	int inodeTableBlockShift = inumber / inodesPerBlock;

	struct inode buffer[inodesPerBlock];
	int numBytes = diskimg_readsector(fileDescriptor, INODE_START_SECTOR + inodeTableBlockShift, buffer);
	if (numBytes == -1) return -1;

	int inodeTableInodeShift = inumber % inodesPerBlock; 
	*inp = buffer[inodeTableInodeShift];
	
	return 0;	
}

/**
 * Given an index of a file block, retrieves the file's actual block number
 * of from the given inode.
 *
 * Returns the disk block number on success, -1 on error.  
 */
int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp, int blockNum) {

	bool large = ( (inp->i_mode & ILARG) != 0 );
	int fileDescriptor = fs->dfd;
	int numBytes;
	int shortsPerBlock = DISKIMG_SECTOR_SIZE / sizeof(short);
	if (large) {
		int bucket = blockNum / shortsPerBlock;
		int shift = blockNum % shortsPerBlock;
		short indirectBlockAddressBuffer[shortsPerBlock];
		if (bucket < 7) {
			//singly indirect
			numBytes = diskimg_readsector(fileDescriptor, inp->i_addr[bucket], indirectBlockAddressBuffer);
			if (numBytes == -1) return -1;	

			return indirectBlockAddressBuffer[shift];

		} else {
			//doubly indirect
			bucket = bucket - 7;	
			short doublyIndirectBlockAddressBuffer[DISKIMG_SECTOR_SIZE];	
			numBytes = diskimg_readsector(fileDescriptor, inp->i_addr[7], doublyIndirectBlockAddressBuffer);
			if (numBytes == -1) return -1;
			
			short indirectBlockNumber = doublyIndirectBlockAddressBuffer[bucket];
			
			numBytes = diskimg_readsector(fileDescriptor, indirectBlockNumber, indirectBlockAddressBuffer);
                        if (numBytes == -1) return -1;
		
			return indirectBlockAddressBuffer[shift];
		}

	} else {
		//small 
		if (blockNum > 7) return -1;
		return inp->i_addr[blockNum];
	}	
}

/**
 * Computes the size in bytes of the file identified by the given inode
 */
int inode_getsize(struct inode *inp) {
	return ((inp->i_size0 << 16) | inp->i_size1); 
}
