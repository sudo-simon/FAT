#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
// My headers
#include "disk.h"
#include "fat.h"
#include "file.h"


extern FAT_STRUCT* FAT;


int _DISK_init(DISK_STRUCT* DISK, char* session_filename){

    // Existing session file to be opened
    if (session_filename != NULL) {
        if(access(session_filename, F_OK)){
            return -1;
        }

        DISK->sessionFd = open(
            session_filename,
            0,
            O_RDWR
        );

        DISK->disk = mmap(
            NULL,
            DISK_SIZE,
            PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_POPULATE,
            DISK->sessionFd,
            0
        );

        DISK->mmappedFlag = 1;
    }

    // DISK to be created from scratch
    else{
        DISK->disk = calloc(DISK_SIZE, sizeof(char));
        DISK->sessionFd = -1;
        DISK->mmappedFlag = 0;
    }

    return 0;
}


int _DISK_destroy(DISK_STRUCT* DISK){
    
    if (DISK->mmappedFlag){
        if(munmap(DISK->disk, DISK_SIZE))
            return -1;
    }
    else free(DISK->disk);
    
    free(DISK);
    return 0;
}


int _DISK_allocateBlock(DISK_STRUCT* DISK, int block_index){
    int start_index = block_index*BLOCK_SIZE;
    for (long i=0; i<BLOCK_SIZE; ++i){
        DISK->disk[start_index+i] = 0;
    }
    //memcpy(DISK->disk+start_index, "", BLOCK_SIZE);
    return 0;
}


int _DISK_writeBytes(DISK_STRUCT* DISK, int block_index, char* buffer, int n_bytes){

    if (block_index >= (DISK_SIZE/BLOCK_SIZE)) return -1;

    int b_index = block_index;
    int disk_index = block_index*BLOCK_SIZE;
    int n_blocks = n_bytes/BLOCK_SIZE;
    if (n_bytes%BLOCK_SIZE > 0) ++n_blocks;

    int next_block_index;

    for (int block=0; block<n_blocks; ++block){

        if (_FAT_allocateBlock(FAT, b_index)) return -1;
        if (_DISK_allocateBlock(DISK, b_index)) return -1;

        memcpy(DISK->disk+disk_index, buffer+(block*BLOCK_SIZE), BLOCK_SIZE);

        if (block < n_blocks-1){
            next_block_index = _FAT_findFirstFreeBlock(FAT);
            _FAT_setNextBlock(FAT, b_index, next_block_index);
            b_index = next_block_index;
            disk_index = next_block_index*BLOCK_SIZE;
        } 
    }

    return n_blocks;

}


char* _DISK_readBytes(DISK_STRUCT* DISK, int block_index, int n_bytes){

    if (block_index >= (DISK_SIZE/BLOCK_SIZE)) return NULL;

    int b_index = block_index;
    int disk_index = block_index*BLOCK_SIZE;
    int n_blocks = n_bytes/BLOCK_SIZE;
    if (n_bytes%BLOCK_SIZE > 0) ++n_blocks;

    int next_block_index;

    char* return_buffer = calloc(n_bytes,sizeof(char));
    for (int block=0; block<n_blocks; ++block){

        //TODO: write this :)

    }
    
    return return_buffer;

}