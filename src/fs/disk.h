#pragma once
#include "../constants.h"


/*
DISK buffer struct.
char* disk,
char mmappedFlag,
int sessionFd
*/
typedef struct DISK_STRUCT {
    char disk[DISK_SIZE];
    char mmappedFlag;
    char sessionFileName[64];
} DISK_STRUCT;


/*
Initializes the DISK struct from scratch, or mmaps it if a session file is passed
*/
int _DISK_init(DISK_STRUCT* DISK, char* session_filename);


/*
Deallocates DISK from memory
*/
int _DISK_destroy(DISK_STRUCT* DISK);


/*
Allocates a block on DISK at index block_index.
Returns 0 if successful, -1 otherwise
*/
int _DISK_allocateBlock(DISK_STRUCT* DISK, int block_index);


/*
Deallocates a block on DISK at index block_index.
Returns 0 if successful, -1 otherwise
*/
// DEAD FUNCTION?
int _DISK_deallocateBlock(DISK_STRUCT* DISK, int block_index);


/*
Writes n_bytes of buffer to DISK starting from block_index.
Returns number of blocks written if successful, -1 otherwise
*/
int _DISK_writeBytes(DISK_STRUCT* DISK, int block_index, char* buffer, int n_bytes);


/*
Reads n_bytes from DISK starting from block_index and returns a buffer
*/
char* _DISK_readBytes(DISK_STRUCT* DISK, int block_index, int n_bytes);

