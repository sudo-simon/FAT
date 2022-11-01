#pragma once
#include "../constants.h"
#include "disk.h"

#define FAT_SIZE sizeof(struct FAT_STRUCT)
#define FAT_ENTRIES DISK_SIZE/BLOCK_SIZE
#define FAT_RESERVED_BLOCKS FAT_SIZE/BLOCK_SIZE +1

/*
File Allocation Table struct.
Size = 5*(DISK_SIZE/BLOCK_SIZE)
*/
typedef struct FAT_STRUCT {
    char isFull[DISK_SIZE/BLOCK_SIZE];
    int next[DISK_SIZE/BLOCK_SIZE];
} FAT_STRUCT;


/*
Initializes the FAT on DISK at startup, and returns the index of the first usable
block on DISK not reserved to the FAT.
If DISK is mmapped then it puts the disk FAT struct in the FAT parameter
*/
int _FAT_init(FAT_STRUCT* FAT, DISK_STRUCT* DISK, char MMAPPED_DISK_FLAG);


/*
Deallocates FAT from memory
*/
void _FAT_destroy(FAT_STRUCT* FAT);


/*
Returns 1 if the file has no next block, or 0 otherwise
*/
char _FAT_EOF(FAT_STRUCT* FAT, int block_index);


/*
Returns the index of the successive block of DISK[block_index] on disk
*/
int _FAT_getNextBlock(FAT_STRUCT* FAT, int block_index);


/*
Sets the <index> block of the FAT to the value <value> (next file block).
Returns 0 if successful
*/
int _FAT_setNextBlock(FAT_STRUCT* FAT, int block_index, int next_index);


/*
Returns the index of the first free block on DISK
*/
int _FAT_findFirstFreeBlock(FAT_STRUCT* FAT);


/*
Sets the value isFull of the corresponding block to 1.
Returns 0 if successful, -1 otherwise
*/
int _FAT_allocateBlock(FAT_STRUCT* FAT, int block_index);


/*
Sets the value isFull of the corresponding block to 0
*/
void _FAT_deallocateBlock(FAT_STRUCT* FAT, int block_index);