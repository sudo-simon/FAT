#include <stdlib.h>
#include <string.h>
#include <stdio.h>
//My Headers
#include "disk.h"
#include "fat.h"
#include "file.h"


FolderHandle* _FILE_initRoot(DISK_STRUCT* DISK, FAT_STRUCT* FAT){

    FolderObject* root = calloc(1, sizeof(struct FolderObject));
    root->isFolder = 1;
    strncpy(root->folderName, "root", MAX_FILENAME_LEN);
    root->nextBlockIndex = -1;
    root->size = 0;
    root->previousFolderBlockIndex = -1;
    for (int i=0; i<(sizeof(root->contentListBlocks)/4); ++i){
        root->contentListBlocks[i] = -1;
    }

    FolderHandle* root_handle = calloc(1,sizeof(struct FolderHandle));
    strncpy(root_handle->folderName, "root", MAX_FILENAME_LEN);
    root_handle->firstBlockIndex = FAT_RESERVED_BLOCKS;
    root_handle->currentBlockIndex = FAT_RESERVED_BLOCKS;
    root_handle->size = 0;
    root_handle->previousFolderBlockIndex = -1;
    root_handle->folderList = calloc(0, sizeof(struct FolderObject*));
    root_handle->fileList = calloc(0,sizeof(struct FileObject*));
    
    _DISK_writeBytes(DISK, FAT_RESERVED_BLOCKS, (char*)root, sizeof(*root));
    
    free(root);
    return root_handle;
}


FolderHandle* _FILE_getRoot(DISK_STRUCT* DISK, FAT_STRUCT* FAT){

    FolderObject* root = (FolderObject*) _DISK_readBytes(DISK, FAT_RESERVED_BLOCKS, sizeof(struct FolderObject));
    FolderHandle* root_handle = calloc(1, sizeof(FolderHandle));
    
    strncpy(root_handle->folderName, root->folderName, MAX_FILENAME_LEN);
    root_handle->firstBlockIndex = FAT_RESERVED_BLOCKS;
    root_handle->currentBlockIndex = FAT_RESERVED_BLOCKS;
    root_handle->size = root->size;
    root_handle->previousFolderBlockIndex = root->previousFolderBlockIndex;
    root_handle->folderList = _FILE_getContainedFolders(DISK, FAT, root);
    root_handle->fileList = _FILE_getContainedFiles(DISK, FAT, root);

    free(root);
    return root_handle;
}


void _FILE_folderHandleDestroy(FolderHandle* folder_handle){
    free(folder_handle->folderList);
    free(folder_handle->fileList);
    free(folder_handle);
    return;
}


void _FILE_fileHandleDestroy(FileHandle* file_handle){
    free(file_handle);
    return;
}


FolderObject** _FILE_getContainedFolders(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderObject* folder){

    int n_folders = 0;
    int* folder_blocks_i = calloc(folder->size,sizeof(int));
    for (int i=0; i<folder->size; ++i){
        folder_blocks_i[i] = -1;
    }

    int next_index = folder->nextBlockIndex;
    int b_index = -1;
    char block_traversed_flag = 0;
    int folder_block[128];

    for (int i=0; i<folder->size; ++i){
        
        // End of block reached, trying to read from the next one
        if ((i-116)%128 == 0){
            b_index = next_index;
            next_index = _FAT_getNextBlock(FAT, folder->nextBlockIndex);
            block_traversed_flag = 1;
            char* tmp_buf = _DISK_readBytes(DISK, b_index, BLOCK_SIZE);
            memcpy(folder_block, tmp_buf, BLOCK_SIZE);
            free(tmp_buf);
        }

        // I am reading the first block of the folder
        if (! block_traversed_flag){
            char* disk_object = _DISK_readBytes(DISK, folder->contentListBlocks[i], BLOCK_SIZE);
            // First byte of the Object is the isFolder flag
            if (disk_object[0] == 1){
                folder_blocks_i[n_folders++] = folder->contentListBlocks[i];
            }
            free(disk_object);
        }
        
        // I am reading a successive block of the folder
        else{
            char* disk_object = _DISK_readBytes(
                DISK, 
                folder_block[(i-116)%128], 
                BLOCK_SIZE
            );
            // First byte of the Object is the isFolder flag
            if (disk_object[0] == 1){
                folder_blocks_i[n_folders++] = folder_block[(i-116)%128];
            }
            free(disk_object);
        }

    }

    FolderObject** folder_list = calloc(n_folders, sizeof(FolderObject*));
    //TODO: ordinarli in ordine alfabetico per permettere una binary search per la _seek

    int i = 0;
    while ((i < folder->size) && (folder_blocks_i[i] != -1)){
        FolderObject* p = (FolderObject*) DISK->disk + (folder_blocks_i[i]*BLOCK_SIZE);
        folder_list[i] = p;
        ++i;
    }

    free(folder_blocks_i);
    return folder_list;

}


FileObject** _FILE_getContainedFiles(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderObject* folder){

    int n_files = 0;
    int* file_blocks_i = calloc(folder->size, sizeof(int));
    for (int i=0; i<folder->size; ++i){
        file_blocks_i[i] = -1;
    }

    int next_index = folder->nextBlockIndex;
    int b_index = -1;
    char block_traversed_flag = 0;
    int folder_block[128];

    for (int i=0; i<folder->size; ++i){

        // End of block reached, trying to read from the next one
        if ((i-116)%128 == 0){
            b_index = next_index;
            next_index = _FAT_getNextBlock(FAT, folder->nextBlockIndex);
            block_traversed_flag = 1;
            char* tmp_buf = _DISK_readBytes(DISK, b_index, BLOCK_SIZE);
            memcpy(folder_block, tmp_buf, BLOCK_SIZE);
            free(tmp_buf);
        }

        // I am reading the first block of the folder
        if (! block_traversed_flag){
            char* disk_object = _DISK_readBytes(DISK, folder->contentListBlocks[i], BLOCK_SIZE);
            // First byte of the Object is the isFolder flag
            if (disk_object[0] == 0){
                file_blocks_i[n_files++] = folder->contentListBlocks[i];
            }
            free(disk_object);
        }
        
        // I am reading a successive block of the folder
        else{
            char* disk_object = _DISK_readBytes(
                DISK, 
                folder_block[(i-116)%128], 
                BLOCK_SIZE
            );
            // First byte of the Object is the isFolder flag
            if (disk_object[0] == 0){
                file_blocks_i[n_files++] = folder_block[(i-116)%128];
            }
            free(disk_object);
        }

    }

    FileObject** file_list = calloc(n_files, sizeof(FileObject*));
    //TODO: ordinarli in ordine alfabetico per permettere una binary search per la _seek

    int i = 0;
    while ((i < folder->size) && (file_blocks_i[i] != -1)){
        FileObject* p = (FileObject*) DISK->disk + (file_blocks_i[i]*BLOCK_SIZE);
        file_list[i] = p;
        ++i;
    }

    free(file_blocks_i);
    return file_list;

}


int _FILE_createFile(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* file_name){
    
    FileObject* new_file = calloc(1, sizeof(struct FileObject));
    new_file->isFolder = 0;
    strncpy(new_file->fileName, file_name, MAX_FILENAME_LEN);
    new_file->nextBlockIndex = -1;
    new_file->size = 0;
    memset(new_file->firstDataBlock, 0, BLOCK_SIZE-44);

    int block = _FAT_findFirstFreeBlock(FAT);
    if (block == -1){
        printf("[ERROR] No space left on DISK!\n");
        return -1;
    }
    if (_FAT_allocateBlock(FAT, block) != 0) return -1;
    
    if (_DISK_writeBytes(DISK, block, (char*) new_file, sizeof(struct FileObject)) == -1) return -1;
    if (_FILE_folderAddFile(DISK, FAT, CWD, block) == -1) return -1;
    
    free(new_file);
    return block;

}


int _FILE_folderAddFile(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, int file_first_block){

    // I need to allocate another block for the folder
    if ((CWD->size-116)%128 == 0){

        int next_folder_block[128];
        memset(next_folder_block, 0, BLOCK_SIZE);
        next_folder_block[0] = file_first_block;

        int next_block_index = _FAT_findFirstFreeBlock(FAT);
        if (next_block_index == -1){
            printf("[ERROR] No space left on DISK!\n");
            return -1;
        }
        if (_FAT_allocateBlock(FAT, next_block_index) != 0) return -1;
        _FAT_setNextBlock(FAT, CWD->currentBlockIndex, next_block_index);
        
        if (_DISK_writeBytes(DISK, next_block_index, (char*) next_folder_block, BLOCK_SIZE) == -1) return -1;
        CWD->currentBlockIndex = next_block_index;

    }

    else{
        //TODO: THIS! Differenziare tra aggiunta al blocco base o ad un blocco successivo
        if (CWD->size < 116){
            // Opero sul blocco del tipo FolderObject*
        }
        else{
            // Opero sul blocco del tipo int Folder_block[128]
        }
    }

    CWD->size++;
    CWD->fileList = realloc(CWD->fileList, (CWD->size * sizeof(struct FolderObject*)));
    CWD->fileList[CWD->size-1] = (FileObject*) DISK->disk + (file_first_block*BLOCK_SIZE);

    return 0;
}
