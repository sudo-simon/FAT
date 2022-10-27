#include <stdlib.h>
#include <string.h>
// My headers
#include "fat.h"

extern FAT_STRUCT* FAT;
extern char* DISK;


int _FAT_init(char MMAPPED_DISK_FLAG) {

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
        memcpy(DISK, FAT, FAT_SIZE);
    }
    else{
        memcpy(FAT, DISK, FAT_SIZE);
    }
    return FAT_RESERVED_BLOCKS;
}


char _FAT_EOF(int disk_index){
    if (FAT->next[disk_index] == -1)
        return 1;
    else
        return 0;
}


char* _FAT_getNextBlock(int disk_index){
    int next_i = FAT->next[disk_index];
    return DISK+(next_i*BLOCK_SIZE);
}


int _FAT_setNextBlock(int disk_index, int next_index){
    FAT->next[disk_index] = next_index;
    FAT->next[next_index] = -1;
    FAT->isFull[next_index] = 1;
    return 0;
}


int _FAT_findFirstFreeBlock(){
    int index = FAT_RESERVED_BLOCKS;
    while (index<FAT_ENTRIES){
        if (FAT->isFull[index] == 0) break;
        ++index;
    }
    if (index == FAT_ENTRIES) return -1;
    return index;
}


void _FAT_deleteBlock(int disk_index){
    FAT->isFull[disk_index] = 0;
    return;
}