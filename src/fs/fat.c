#include <stdlib.h>
#include <string.h>
// My headers
#include "disk.h"
#include "fat.h"


int _FAT_init(FAT_STRUCT* FAT, DISK_STRUCT* DISK, char MMAPPED_DISK_FLAG){

    if (!MMAPPED_DISK_FLAG){
        for (int i=0; i<FAT_ENTRIES; ++i){
            if (i < FAT_RESERVED_BLOCKS){
                FAT->isFull[i] = 1;
                if (i != FAT_RESERVED_BLOCKS-1) FAT->next[i] = i+1;
                else FAT->next[i] = -1;
            }
            else{
                FAT->isFull[i] = 0;
                FAT->next[i] = -1;
            }
        }
        memcpy(DISK->disk, FAT, FAT_SIZE);
    }

    else{
        memcpy(FAT, DISK->disk, FAT_SIZE);
    }

    return FAT_RESERVED_BLOCKS;
}


void _FAT_destroy(FAT_STRUCT* FAT){
    free(FAT);
}


char _FAT_EOF(FAT_STRUCT* FAT, int block_index){
    if (FAT->next[block_index] == -1) return 1;
    else return 0;
}


int _FAT_getNextBlock(FAT_STRUCT* FAT, int block_index){
    return FAT->next[block_index];
}


int _FAT_setNextBlock(FAT_STRUCT* FAT, int block_index, int next_index){
    FAT->next[block_index] = next_index;
    return 0;
}

/*
Returns the index of the first free block on DISK
*/
int _FAT_findFirstFreeBlock(FAT_STRUCT* FAT){
    int index = FAT_RESERVED_BLOCKS;
    while (index<FAT_ENTRIES){
        if (FAT->isFull[index] == 0) break;
        ++index;
    }
    if (index == FAT_ENTRIES) return -1;
    return index;
}


int _FAT_allocateBlock(FAT_STRUCT* FAT, int block_index){
    if (FAT->isFull[block_index] == 0){
        FAT->isFull[block_index] = 1;
        FAT->next[block_index] = -1;
        return 0; 
    }     
    else return -1;
}


void _FAT_deallocateBlock(FAT_STRUCT* FAT, int block_index){
    FAT->isFull[block_index] = 0;
    FAT->next[block_index] = -1;
    return;
}


int _FAT_writeOnDisk(FAT_STRUCT* FAT, DISK_STRUCT* DISK){
    if (_DISK_writeBytes(DISK, 0, (char*) FAT, FAT_SIZE) == -1) return -1;
    return 0;
}