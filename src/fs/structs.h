#pragma once
#include "../constants.h"

//FAT as a fixed size buffer
typedef int FAT_buffer[DISK_SIZE/BLOCK_SIZE];

// FAT as a struct
typedef struct FAT_struct {
    int table[DISK_SIZE/BLOCK_SIZE];
    char free_slots[DISK_SIZE/BLOCK_SIZE]; //bitmap? 
} FAT_struct;


/*
FileHandle struct returned to the user editing a file
*/
typedef struct FileHandle {
    // TODO
} FileHandle;


/*
Returns a pointer to the successive block of DISK[disk_index] on disk
*/
char* _FAT_getNextBlock(int* FAT, int disk_index);

/*
Sets the <index> block of the FAT to the value <value> (next file block)
*/
int _FAT_setBlock(int* FAT, int disk_index, int value);
