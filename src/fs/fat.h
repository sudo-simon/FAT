#pragma once
#include "../constants.h"

#define FAT_SIZE sizeof(struct FAT_STRUCT)
#define FAT_ENTRIES DISK_SIZE/BLOCK_SIZE
#define FAT_RESERVED_BLOCKS FAT_SIZE/BLOCK_SIZE +1

/*
File Allocation Table struct,
Size = 33*(DISK_SIZE/BLOCK_SIZE)
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
int _FAT_init(char MMAPPED_DISK_FLAG);


/*
Returns 1 if the file has no next block, or 0 otherwise
*/
char _FAT_EOF(int disk_index);


/*
Returns a pointer to the successive block of DISK[disk_index] on disk
*/
char* _FAT_getNextBlock(int disk_index);


/*
Sets the <index> block of the FAT to the value <value> (next file block).
Returns 0 if successful
*/
int _FAT_setNextBlock(int disk_index, int next_index);


/*
Returns the index of the first free block on DISK
*/
int _FAT_findFirstFreeBlock();


/*
Sets the value isFull of the corresponding block to 0
*/
void _FAT_deleteBlock(int disk_index);