#include <stdio.h>
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
        if(access(session_filename, R_OK | W_OK) != 0){
            return -1;
        }

        strncpy(DISK->sessionFileName, session_filename, 64);

        FILE* f = fopen(session_filename, "r+");
        if (fread(DISK->disk, 1, DISK_SIZE, f) == 0){
            return -1;
        }
        fclose(f);

        DISK->persistentFlag = 1;
    }

    // DISK to be created from scratch
    else{
        memset(DISK->disk, 0, DISK_SIZE);
        strncpy(DISK->sessionFileName, "", 64);
        DISK->persistentFlag = 0;
    }

    return 0;
}


int _DISK_destroy(DISK_STRUCT* DISK){  
    if (DISK->persistentFlag){
        if (_FAT_writeOnDisk(FAT, DISK) == -1) return -1;
        FILE* f =  fopen(DISK->sessionFileName, "w");
        if (fwrite(DISK->disk, 1, DISK_SIZE, f) == 0) return -1;
        fclose(f);
    }
    free(DISK);
    return 0;
}


int _DISK_allocateBlock(DISK_STRUCT* DISK, int block_index){
    int start_index = block_index*BLOCK_SIZE;
    memset(DISK->disk+start_index, 0, BLOCK_SIZE);
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

        if (_DISK_allocateBlock(DISK, b_index)) return -1;

        memcpy(DISK->disk+disk_index, buffer+(block*BLOCK_SIZE), BLOCK_SIZE);

        if (block < n_blocks-1){
            next_block_index = _FAT_findFirstFreeBlock(FAT);
            if (_FAT_allocateBlock(FAT, next_block_index) != 0) return -1;
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

        memcpy(return_buffer+(block*BLOCK_SIZE), DISK->disk+disk_index, BLOCK_SIZE);

        if (block < n_blocks-1){
            if (_FAT_EOF(FAT, b_index)) return return_buffer;
            next_block_index = _FAT_getNextBlock(FAT, b_index);
            b_index = next_block_index;
            disk_index = next_block_index*BLOCK_SIZE;
        }

    }
    
    return return_buffer;

}