#include <stdlib.h>
#include <string.h>
//My Headers
#include "file.h"


FileHandle* _FILE_initRoot(DISK_STRUCT* DISK, FAT_STRUCT* FAT){

    FileObject* root = calloc(1, sizeof(struct FileObject));
    strncpy(root->fileName, "/", MAX_FILENAME_LEN);
    root->isFolder = 1;
    root->folderFiles = calloc(0,sizeof(FileObject));
    root->firstDiskBlockIndex = FAT_RESERVED_BLOCKS;
    root->size = 0;
    root->data = NULL;

    FileHandle* root_handle = calloc(1,sizeof(struct FileHandle));
    strncpy(root_handle->fileName, "/", MAX_FILENAME_LEN);
    root_handle->firstDiskBlockIndex = root->firstDiskBlockIndex;
    root_handle->currentDiskBlockIndex = root_handle->firstDiskBlockIndex;
    root_handle->size = 0;

    _DISK_writeBytes(DISK, root->firstDiskBlockIndex, (char*)root, sizeof(*root));
    
    free(root);
    return root_handle;
}


FileHandle* _FILE_getRoot(DISK_STRUCT* DISK, FAT_STRUCT* FAT){

    FileObject* root = (FileObject*) _DISK_readBytes(DISK, FAT_RESERVED_BLOCKS, sizeof(struct FileObject));
    FileHandle* root_handle = calloc(1, sizeof(FileHandle));
    
    strncpy(root_handle->fileName, root->fileName, MAX_FILENAME_LEN);
    root_handle->firstDiskBlockIndex = root->firstDiskBlockIndex;
    root_handle->currentDiskBlockIndex = root_handle->firstDiskBlockIndex;
    root_handle->size = root->size;

    free(root);
    return root_handle;
}



