#include <stdlib.h>
#include <string.h>
//My Headers
#include "disk.h"
#include "file.h"


FolderHandle* _FILE_initRoot(DISK_STRUCT* DISK, FAT_STRUCT* FAT){

    FolderObject* root = calloc(1, sizeof(struct FolderObject));
    root->isFolder = 1;
    strncpy(root->folderName, "root", MAX_FILENAME_LEN);
    root->nextBlockIndex = -1;
    root->size = 0;
    for (int i=0; i<(sizeof(root->contentListBlocks)/4); ++i){
        root->contentListBlocks[i] = -1;
    }

    FolderHandle* root_handle = calloc(1,sizeof(struct FolderHandle));
    strncpy(root_handle->folderName, "root", MAX_FILENAME_LEN);
    root_handle->firstBlockIndex = FAT_RESERVED_BLOCKS;
    root_handle->currentBlockIndex = FAT_RESERVED_BLOCKS;
    root_handle->size = 0;
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
    root_handle->folderList = root->contentListBlocks;
    root_handle->fileList = root->contentListBlocks;

    free(root);
    return root_handle;
}


void*** _FILE_getContentPointers(DISK_STRUCT* DISK, int* content_blocks, int folder_size){

    void*** ret_pair = calloc(2,sizeof(void**));

    int n_folders = 0;
    int* folder_indexes = calloc(folder_size,sizeof(int));
    int n_files = 0;
    int* file_indexes = calloc(folder_size, sizeof(int));
    for (int i=0; i<folder_size; ++i){
        folder_indexes[i] = -1;
        file_indexes[i] = -1;
    }

    for (int i=0; i<folder_size; ++i){
        char* disk_object = _DISK_readBytes(DISK, content_blocks[i], BLOCK_SIZE);
        // First byte of the Object is the isFolder flag
        if (disk_object[0] == 1){
            folder_indexes[n_folders++] = i;
        }
        else{
            file_indexes[n_files++] = i;
        }
        free(disk_object);
    }

    FolderObject** folder_list = calloc(n_folders, sizeof(FolderObject*));
    FileObject** file_list = calloc(n_files, sizeof(FileObject*));

    for (int folder_i=0; folder_i<n_folders; ++folder_i){

    }
    for (int file_i=0; file_i<n_files; ++file_i){
        
    }

    ret_pair[0] = (void**) folder_list;
    ret_pair[1] = (void**) file_list;
    return ret_pair;

}



