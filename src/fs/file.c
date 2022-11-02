#include <stdlib.h>
#include <string.h>
#include <stdio.h>
//My Headers
#include "disk.h"
#include "fat.h"
#include "file.h"
#include "aux.h"


/* DEAD CODE
int _FILE_contentListBlocks_comparator(const void* n1, const void* n2){
    int a = *((int*) n1);
    int b = *((int*) n2);
    return a < b ? -1 : a > b ? +1 : 0;
}

int _FILE_FolderListElem_comparator(const void* n1, const void* n2){
    FolderListElem* e1 = (FolderListElem*) n1;
    FolderListElem* e2 = (FolderListElem*) n2;
    return strncmp(e1->name, e2->name, MAX_FILENAME_LEN);
}
*/


FolderHandle* _FILE_initRoot(DISK_STRUCT* DISK, FAT_STRUCT* FAT){

    FolderObject* root = calloc(1, sizeof(struct FolderObject));
    root->isFolder = 1;
    strncpy(root->folderName, "root", MAX_FILENAME_LEN);
    root->nextBlockIndex = -1;
    root->size = 0;
    root->numFolders = 0;
    root->numFiles = 0;
    root->previousFolderBlockIndex = -1;
    for (int i=0; i<CONTENT_LIST_BLOCKS_SIZE; ++i){
        root->contentListBlocks[i] = -1;
    }

    FolderHandle* root_handle = calloc(1,sizeof(struct FolderHandle));
    strncpy(root_handle->folderName, "root", MAX_FILENAME_LEN);
    root_handle->firstBlockIndex = FAT_RESERVED_BLOCKS;
    root_handle->currentBlockIndex = FAT_RESERVED_BLOCKS;
    root_handle->size = 0;
    root_handle->previousFolderBlockIndex = -1;
    root_handle->numFolders = 0;
    root_handle->folderList = calloc(0, sizeof(struct FolderListElem));
    root_handle->numFiles = 0;
    root_handle->fileList = calloc(0,sizeof(struct FolderListElem));
    
    _FAT_allocateBlock(FAT, FAT_RESERVED_BLOCKS);
    _DISK_writeBytes(DISK, FAT_RESERVED_BLOCKS, (char*)root, sizeof(*root));
    
    free(root);
    return root_handle;
}


FolderHandle* _FILE_getRoot(DISK_STRUCT* DISK, FAT_STRUCT* FAT){

    FolderObject* root = (FolderObject*) _DISK_readBytes(DISK, FAT_RESERVED_BLOCKS, sizeof(struct FolderObject));
    FolderHandle* root_handle = calloc(1, sizeof(FolderHandle));
    
    strncpy(root_handle->folderName, root->folderName, MAX_FILENAME_LEN);
    root_handle->firstBlockIndex = FAT_RESERVED_BLOCKS;

    int b_index = FAT_RESERVED_BLOCKS;
    int next_index = _FAT_getNextBlock(FAT, b_index);
    while (b_index != -1){
        root_handle->currentBlockIndex = b_index;
        b_index = next_index;
        next_index = _FAT_getNextBlock(FAT, b_index);
    }

    root_handle->size = root->size;
    root_handle->previousFolderBlockIndex = root->previousFolderBlockIndex;
    root_handle->numFolders = root->numFolders;
    root_handle->folderList = _FILE_getContainedFolders(DISK, FAT, root);
    root_handle->numFiles = root->numFiles;
    root_handle->fileList = _FILE_getContainedFiles(DISK, FAT, root);

    free(root);
    return root_handle;
}


void _FILE_folderHandleDestroy(FolderHandle* folder_handle){
    for (int i=0; i<folder_handle->numFolders; ++i)
        free(folder_handle->folderList[i]);
    for (int i=0; i<folder_handle->numFiles; ++i)
        free(folder_handle->fileList[i]);
    free(folder_handle->folderList);
    free(folder_handle->fileList);
    free(folder_handle);
    return;
}


void _FILE_fileHandleDestroy(FileHandle* file_handle){
    free(file_handle);
    return;
}






FolderListElem** _FILE_getContainedFolders(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderObject* folder){

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
        if ((i-CONTENT_LIST_BLOCKS_SIZE)%128 == 0){
            b_index = next_index;
            next_index = _FAT_getNextBlock(FAT, next_index);
            block_traversed_flag = 1;
            char* tmp_buf = _DISK_readBytes(DISK, b_index, BLOCK_SIZE);
            memcpy(folder_block, tmp_buf, BLOCK_SIZE);
            free(tmp_buf);
        }

        // I am reading the first block of the folder
        if (! block_traversed_flag){
            //if (folder->contentListBlocks[i] < 0) continue;
            char* disk_object = _DISK_readBytes(DISK, folder->contentListBlocks[i], BLOCK_SIZE);
            // First byte of the Object is the isFolder flag
            if (disk_object[0] == 1){
                folder_blocks_i[n_folders++] = folder->contentListBlocks[i];
            }
            free(disk_object);
        }
        
        // I am reading a successive block of the folder
        else{
            if (folder_block[(i-CONTENT_LIST_BLOCKS_SIZE)%128] == -1) continue;
            char* disk_object = _DISK_readBytes(
                DISK, 
                folder_block[(i-CONTENT_LIST_BLOCKS_SIZE)%128], 
                BLOCK_SIZE
            );
            // First byte of the Object is the isFolder flag
            if (disk_object[0] == 1){
                folder_blocks_i[n_folders++] = folder_block[(i-CONTENT_LIST_BLOCKS_SIZE)%128];
            }
            free(disk_object);
        }

    }

    FolderListElem** folder_list = calloc(n_folders, sizeof(struct FolderListElem*));

    int i = 0;
    while ((i < folder->numFolders) && (folder_blocks_i[i] != -1)){
        FolderObject* f = (FolderObject*) _DISK_readBytes(DISK, folder_blocks_i[i], sizeof(struct FolderObject));
        
        FolderListElem* e = calloc(1,sizeof(struct FolderListElem));
        strncpy(e->name, f->folderName, MAX_FILENAME_LEN);
        e->firstBlockIndex = folder_blocks_i[i];
        e->size = f->size;
        folder_list[i] = e;

        free(f);
        ++i;
    }

    // Alphabetically sorting folder_list
    _AUX_alphabeticalSort(folder_list, n_folders);

    free(folder_blocks_i);
    return folder_list;

}


FolderListElem** _FILE_getContainedFiles(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderObject* folder){

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
        if ((i-CONTENT_LIST_BLOCKS_SIZE)%128 == 0){
            b_index = next_index;
            next_index = _FAT_getNextBlock(FAT, next_index);
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
            if (folder_block[(i-CONTENT_LIST_BLOCKS_SIZE)%128] == -1) continue;
            char* disk_object = _DISK_readBytes(
                DISK, 
                folder_block[(i-CONTENT_LIST_BLOCKS_SIZE)%128], 
                BLOCK_SIZE
            );
            // First byte of the Object is the isFolder flag
            if (disk_object[0] == 0){
                file_blocks_i[n_files++] = folder_block[(i-CONTENT_LIST_BLOCKS_SIZE)%128];
            }
            free(disk_object);
        }

    }

    FolderListElem** file_list = calloc(n_files, sizeof(struct FolderListElem*));

    int i = 0;
    while ((i < folder->size) && (file_blocks_i[i] != -1)){
        FileObject* f = (FileObject*) _DISK_readBytes(DISK, file_blocks_i[i], sizeof(struct FileObject));
        
        FolderListElem* e = calloc(1,sizeof(struct FolderListElem));
        strncpy(e->name, f->fileName, MAX_FILENAME_LEN);
        e->firstBlockIndex = file_blocks_i[i];
        e->size = f->size;
        file_list[i] = e;

        free(f);
        ++i;
    }

    // Alphabetically sorting file_list
    _AUX_alphabeticalSort(file_list, n_files);

    free(file_blocks_i);
    return file_list;

}






int _FILE_searchFolderInCWD(FolderHandle* CWD, char* folder_name){
    int left = 0;
    int right = CWD->numFolders - 1;
    while (left <= right){
        int mid = left + ((right-left) / 2);
        int res = strncmp(folder_name, CWD->folderList[mid]->name, MAX_FILENAME_LEN);
        if (res == 0) return mid;
        else if (res > 0) left = mid+1;
        else right = mid-1;
    }
    return -1;
}
int _FILE_searchFileInCWD(FolderHandle* CWD, char* file_name){
    int left = 0;
    int right = CWD->numFiles - 1;
    while (left <= right){
        int mid = left + ((right-left) / 2);
        int res = strncmp(file_name, CWD->fileList[mid]->name, MAX_FILENAME_LEN);
        if (res == 0) return mid;
        else if (res > 0) left = mid+1;
        else right = mid-1;
    }
    return -1;
}


char _FILE_existingFileName(FolderHandle* CWD, char* file_name){
    if (_FILE_searchFileInCWD(CWD, file_name) != -1) return 1;
    else return 0;
}

char _FILE_existingFolderName(FolderHandle* CWD, char* folder_name){
    if (_FILE_searchFolderInCWD(CWD, folder_name) != -1) return 1;
    else return 0;
}






int _FILE_createFile(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* file_name){
    
    FileObject* new_file = calloc(1, sizeof(struct FileObject));
    new_file->isFolder = 0;
    strncpy(new_file->fileName, file_name, MAX_FILENAME_LEN);
    new_file->nextBlockIndex = -1;
    new_file->size = 0;
    memset(new_file->firstDataBlock, 0, FIRST_DATA_BLOCK_SIZE);

    int block = _FAT_findFirstFreeBlock(FAT);
    if (block == -1){
        printf("[ERROR] No space left on DISK!\n");
        return -1;
    }
    if (_FAT_allocateBlock(FAT, block) != 0) return -1;
    
    if (_DISK_writeBytes(DISK, block, (char*) new_file, sizeof(struct FileObject)) == -1) return -1;
    if (_FILE_folderAddFile(DISK, FAT, CWD, new_file, block) == -1) return -1;
    
    free(new_file);
    return block;

}


int _FILE_folderAddFile(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, FileObject* new_file, int file_first_block){

    int next_block_index = -1;

    // I need to allocate another block for the folder
    if ((CWD->size-CONTENT_LIST_BLOCKS_SIZE)%128 == 0){

        int next_folder_block[128];
        memset(next_folder_block, -1, BLOCK_SIZE);
        next_folder_block[0] = file_first_block;

        next_block_index = _FAT_findFirstFreeBlock(FAT);
        if (next_block_index == -1){
            printf("[ERROR] No space left on DISK!\n");
            return -1;
        }
        if (_FAT_allocateBlock(FAT, next_block_index) != 0) return -1;
        _FAT_setNextBlock(FAT, CWD->currentBlockIndex, next_block_index);
        
        if (_DISK_writeBytes(DISK, next_block_index, (char*) next_folder_block, BLOCK_SIZE) == -1) return -1;
        CWD->currentBlockIndex = next_block_index;

    }

    //TODO: Algorithm check (spooky math here)
    FolderObject* folder_obj = (FolderObject*) _DISK_readBytes(
        DISK, 
        CWD->firstBlockIndex, 
        sizeof(struct FolderObject)
    );
    folder_obj->size++;
    folder_obj->numFiles++;
    
    if (folder_obj->size <= CONTENT_LIST_BLOCKS_SIZE){
        folder_obj->contentListBlocks[folder_obj->size-1] = file_first_block;
        // Mantaining block index sort
        _AUX_blockSort(folder_obj->contentListBlocks, CONTENT_LIST_BLOCKS_SIZE);
    }
    else{
        if (folder_obj->size == (CONTENT_LIST_BLOCKS_SIZE+1)){
            folder_obj->nextBlockIndex = next_block_index;
        }

        int* folder_block = (int*) _DISK_readBytes(DISK, CWD->currentBlockIndex, BLOCK_SIZE);
        folder_block[(folder_obj->size-(CONTENT_LIST_BLOCKS_SIZE+1))%128] = file_first_block;

        // Mantaining block index sort
        _AUX_blockSort(folder_block, 128);

        _DISK_writeBytes(DISK, CWD->currentBlockIndex, (char*) folder_block, BLOCK_SIZE);
        free(folder_block);
    }

    _DISK_writeBytes(DISK, CWD->firstBlockIndex, (char*) folder_obj, sizeof(struct FolderObject));
    free(folder_obj);
    
    CWD->size++;
    CWD->numFiles++;
    CWD->fileList = realloc(CWD->fileList, (CWD->numFiles * sizeof(struct FolderListElem*)));

    FolderListElem* e = calloc(1, sizeof(struct FolderListElem));
    strncpy(e->name, new_file->fileName, MAX_FILENAME_LEN);
    e->firstBlockIndex = file_first_block;
    e->size = new_file->size;
    CWD->fileList[CWD->numFiles-1] = e;

    /*/DEBUG SECTION
    printf("-----> Before sorting:\n");
    for (int i=0; i<CWD->numFiles;  ++i){
        printf("fileList[%d] = %s\n",i,CWD->fileList[i]->name);
    }
    */

    // Making sure the alphabetical sort is mantained
    _AUX_alphabeticalSort(CWD->fileList, CWD->numFiles);

    /*/DEBUG SECTION
    printf("-----> After sorting:\n");
    for (int i=0; i<CWD->numFiles;  ++i){
        printf("fileList[%d] = %s\n",i,CWD->fileList[i]->name);
    }
    */

    return 0;
}






int _FILE_deleteFile(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* file_name){
    
    int cwd_index = _FILE_searchFileInCWD(CWD, file_name);
    if (cwd_index == -1){
        printf("No file named %s\n",file_name);
        return -1;
    }

    FileObject* file_object = (FileObject*) _DISK_readBytes(
        DISK, 
        CWD->fileList[cwd_index]->firstBlockIndex, 
        sizeof(struct FileObject)
    );

    int b_index = CWD->fileList[cwd_index]->firstBlockIndex;
    int next_index = file_object->nextBlockIndex;
    
    // Procedurally deallocate FAT entries
    while (b_index != -1){
        _FAT_deallocateBlock(FAT, b_index);
        b_index = next_index;
        next_index = _FAT_getNextBlock(FAT, next_index);
    }

    if (_FILE_folderRemoveFile(DISK, FAT, CWD, CWD->fileList[cwd_index]->firstBlockIndex) != 0){
        printf("[ERROR] Impossible to remove file from folder!\n");
        return -1;
    }

    // Putting the element to be removed at the end of the list for the realloc
    FolderListElem* tmp = CWD->fileList[cwd_index];
    CWD->fileList[cwd_index] = CWD->fileList[CWD->numFiles-1];
    CWD->fileList[CWD->numFiles-1] = tmp;

    free(CWD->fileList[CWD->numFiles-1]);
    CWD->fileList = realloc(CWD->fileList, ((CWD->numFiles-1) * sizeof(struct FolderListElem*)));
    CWD->size--;
    CWD->numFiles--;

    // Mantaining the alphabetical sorting
    _AUX_alphabeticalSort(CWD->fileList, CWD->numFiles);

    free(file_object);
    return 0;

}


int _FILE_folderRemoveFile(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, int file_first_block){
    
    FolderObject* folder_object = (FolderObject*) _DISK_readBytes(
        DISK, 
        CWD->firstBlockIndex, 
        sizeof(struct FolderObject)
    );

    // Remove the file block index from the FolderObject or a subsequent folder block
    int content_i = -1;
    if (folder_object->size <= CONTENT_LIST_BLOCKS_SIZE){
        content_i = _AUX_intBinarySearch(
            folder_object->contentListBlocks, 
            CONTENT_LIST_BLOCKS_SIZE, 
            file_first_block
        );

        folder_object->contentListBlocks[content_i] = -1;
        _AUX_blockSort(folder_object->contentListBlocks, CONTENT_LIST_BLOCKS_SIZE);   
    }
    else{
        content_i = _AUX_intBinarySearch(
            folder_object->contentListBlocks, 
            CONTENT_LIST_BLOCKS_SIZE, 
            file_first_block
        );
        
        if (content_i != -1){
            folder_object->contentListBlocks[content_i] = -1;
            _AUX_blockSort(folder_object->contentListBlocks, CONTENT_LIST_BLOCKS_SIZE);
        }
        else{
            int b_index = folder_object->nextBlockIndex;
            int next_index = _FAT_getNextBlock(FAT, b_index);
            while (b_index != -1){
                int* folder_block = (int*) _DISK_readBytes(DISK, b_index, BLOCK_SIZE);
                b_index = next_index;
                next_index = _FAT_getNextBlock(FAT, next_index);

                content_i = _AUX_intBinarySearch(
                    folder_block, 
                    128, 
                    file_first_block
                );
                if (content_i == -1){
                    free(folder_block);
                    continue;
                }

                folder_block[content_i] = -1;
                _AUX_blockSort(folder_block, 128);
                _DISK_writeBytes(DISK, b_index, (char*) folder_block, BLOCK_SIZE);
                free(folder_block);
            }
        }
    }

    folder_object->size--;
    folder_object->numFiles--;
    _DISK_writeBytes(DISK, CWD->firstBlockIndex, (char*) folder_object, sizeof(struct FolderObject));

    free(folder_object);
    return 0;

}






int _FILE_createFolder(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* folder_name){

    FolderObject* new_folder = calloc(1, sizeof(struct FolderObject));
    new_folder->isFolder = 1;
    strncpy(new_folder->folderName, folder_name, MAX_FILENAME_LEN);
    new_folder->nextBlockIndex = -1;
    new_folder->size = 0;
    new_folder->numFolders = 0;
    new_folder->numFiles = 0;
    new_folder->previousFolderBlockIndex = CWD->firstBlockIndex;
    for (int i=0; i<CONTENT_LIST_BLOCKS_SIZE; ++i){
        new_folder->contentListBlocks[i] = -1;
    }

    int block = _FAT_findFirstFreeBlock(FAT);
    if (block == -1){
        printf("[ERROR] No space left on DISK!\n");
        return -1;
    }
    if (_FAT_allocateBlock(FAT, block) != 0) return -1;
    
    if (_DISK_writeBytes(DISK, block, (char*) new_folder, sizeof(struct FolderObject)) == -1) return -1;
    if (_FILE_folderAddFolder(DISK, FAT, CWD, new_folder, block) == -1) return -1;
    
    free(new_folder);
    return block;

}


int _FILE_folderAddFolder(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, FolderObject* new_folder, int folder_first_block){

    int next_block_index = -1;

    // I need to allocate another block for the folder
    if ((CWD->size-CONTENT_LIST_BLOCKS_SIZE)%128 == 0){

        int next_folder_block[128];
        memset(next_folder_block, -1, BLOCK_SIZE);
        next_folder_block[0] = folder_first_block;

        next_block_index = _FAT_findFirstFreeBlock(FAT);
        if (next_block_index == -1){
            printf("[ERROR] No space left on DISK!\n");
            return -1;
        }
        if (_FAT_allocateBlock(FAT, next_block_index) != 0) return -1;
        _FAT_setNextBlock(FAT, CWD->currentBlockIndex, next_block_index);
        
        if (_DISK_writeBytes(DISK, next_block_index, (char*) next_folder_block, BLOCK_SIZE) == -1) return -1;
        CWD->currentBlockIndex = next_block_index;

    }

    //TODO: Algorithm check (spooky math here)
    FolderObject* folder_obj = (FolderObject*) _DISK_readBytes(
        DISK, 
        CWD->firstBlockIndex, 
        sizeof(struct FolderObject)
    );
    folder_obj->size++;
    folder_obj->numFolders++;
    
    if (folder_obj->size <= CONTENT_LIST_BLOCKS_SIZE){
        folder_obj->contentListBlocks[folder_obj->size-1] = folder_first_block;
        // Mantaining block index sort
        _AUX_blockSort(folder_obj->contentListBlocks, CONTENT_LIST_BLOCKS_SIZE);
    }
    else{
        if (folder_obj->size == (CONTENT_LIST_BLOCKS_SIZE+1)){
            folder_obj->nextBlockIndex = next_block_index;
        }

        int* folder_block = (int*) _DISK_readBytes(DISK, CWD->currentBlockIndex, BLOCK_SIZE);
        folder_block[(folder_obj->size-(CONTENT_LIST_BLOCKS_SIZE+1))%128] = folder_first_block;

        // Mantaining block index sort
        _AUX_blockSort(folder_block, 128);

        _DISK_writeBytes(DISK, CWD->currentBlockIndex, (char*) folder_block, BLOCK_SIZE);
        free(folder_block);
    }

    _DISK_writeBytes(DISK, CWD->firstBlockIndex, (char*) folder_obj, sizeof(struct FolderObject));
    free(folder_obj);
    
    CWD->size++;
    CWD->numFolders++;
    CWD->folderList = realloc(CWD->folderList, (CWD->numFolders * sizeof(struct FolderListElem*)));

    FolderListElem* e = calloc(1, sizeof(struct FolderListElem));
    strncpy(e->name, new_folder->folderName, MAX_FILENAME_LEN);
    e->firstBlockIndex = folder_first_block;
    e->size = new_folder->size;
    CWD->folderList[CWD->numFolders-1] = e;

    /*/DEBUG SECTION
    printf("-----> Before sorting:\n");
    for (int i=0; i<CWD->numFolders;  ++i){
        printf("folderList[%d] = %s\n",i,CWD->folderList[i]->name);
    }
    */

    // Making sure the alphabetical sort is mantained
    _AUX_alphabeticalSort(CWD->folderList, CWD->numFolders);

    /*/DEBUG SECTION
    printf("-----> After sorting:\n");
    for (int i=0; i<CWD->numFolders;  ++i){
        printf("folderList[%d] = %s\n",i,CWD->folderList[i]->name);
    }
    */

    return 0;

}






int _FILE_deleteFolder(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* folder_name, char recursive_flag){

    int cwd_index = _FILE_searchFolderInCWD(CWD, folder_name);
    if (cwd_index == -1){
        printf("No folder named %s\n",folder_name);
        return -1;
    }

    FolderObject* folder_object = (FolderObject*) _DISK_readBytes(
        DISK, 
        CWD->folderList[cwd_index]->firstBlockIndex, 
        sizeof(struct FolderObject)
    );

    int b_index = CWD->folderList[cwd_index]->firstBlockIndex;
    int next_index = folder_object->nextBlockIndex;
    
    // Recursively deallocates subfolders and files if present
    if (folder_object->size > 0){

        if (! recursive_flag){
            char ans;
            //int ans;
            printf("The folder contains other files and folders.\nDo you want to recursively remove them all? [y/n]: ");
            scanf("%c",&ans);
            //ans = fgetc(stdin);
            if (ans == 'n' || ans == 'N'){
                free(folder_object);
                return 0;
            }
            else if (ans == 'y' || ans == 'Y'){
                _FILE_recursiveFolderDelete(DISK, FAT, CWD, folder_object);
            }
            else{
                printf("%c is not a valid answer\n",ans);
                free(folder_object);
                return 0;
            }
        }

        else _FILE_recursiveFolderDelete(DISK, FAT, CWD, folder_object);

    }

    // Procedurally deallocate FAT entries
    while (b_index != -1){
        _FAT_deallocateBlock(FAT, b_index);
        b_index = next_index;
        next_index = _FAT_getNextBlock(FAT, next_index);
    }

    if (_FILE_folderRemoveFolder(DISK, FAT, CWD, CWD->folderList[cwd_index]->firstBlockIndex) != 0){
        printf("[ERROR] Impossible to remove folder %s from folder!\n",folder_name);
        return -1;
    }

    // Putting the element to be removed at the end of the list for the realloc
    FolderListElem* tmp = CWD->folderList[cwd_index];
    CWD->folderList[cwd_index] = CWD->folderList[CWD->numFolders-1];
    CWD->folderList[CWD->numFolders-1] = tmp;

    free(CWD->folderList[CWD->numFolders-1]);
    CWD->folderList = realloc(CWD->folderList, ((CWD->numFolders-1) * sizeof(struct FolderListElem*)));
    CWD->size--;
    CWD->numFolders--;

    // Mantaining the alphabetical sorting
    _AUX_alphabeticalSort(CWD->folderList, CWD->numFolders);

    free(folder_object);
    return 0;

}


void _FILE_recursiveFolderDelete(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* current_dir, FolderObject* folder_object){

    FolderHandle* folder = calloc(1,sizeof(struct FolderHandle));
    strncpy(folder->folderName, folder_object->folderName, MAX_FILENAME_LEN);
    folder->firstBlockIndex = current_dir->folderList[_FILE_searchFolderInCWD(current_dir, folder_object->folderName)]->firstBlockIndex;
    //folder->currentBlockIndex = ????
    folder->size = folder_object->size;
    folder->previousFolderBlockIndex = folder_object->previousFolderBlockIndex;
    folder->numFolders = folder_object->numFolders;
    folder->folderList = _FILE_getContainedFolders(DISK, FAT, folder_object);
    folder->numFiles = folder_object->numFiles;
    folder->fileList = _FILE_getContainedFiles(DISK, FAT, folder_object);

    // AUX variables used for looping
    int n_folders = folder->numFolders;
    int n_files = folder->numFiles;

    // File deletion
    for (int i=0; i<n_files; ++i){
        _FILE_deleteFile(DISK, FAT, folder, folder->fileList[i]->name);
    }

    // Folder deletion
    for (int i=0; i<n_folders; ++i){
        _FILE_deleteFolder(DISK, FAT, folder, folder->folderList[i]->name, 1);
    }


    free(folder->folderList);
    free(folder->fileList);
    free(folder);
    return;

}


int _FILE_folderRemoveFolder(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, int folder_first_block){

    FolderObject* folder_object = (FolderObject*) _DISK_readBytes(
        DISK, 
        CWD->firstBlockIndex, 
        sizeof(struct FolderObject)
    );

    // Remove the file block index from the FolderObject or a subsequent folder block
    int content_i = -1;
    if (folder_object->size <= CONTENT_LIST_BLOCKS_SIZE){
        content_i = _AUX_intBinarySearch(
            folder_object->contentListBlocks, 
            CONTENT_LIST_BLOCKS_SIZE, 
            folder_first_block
        );

        folder_object->contentListBlocks[content_i] = -1;
        _AUX_blockSort(folder_object->contentListBlocks, CONTENT_LIST_BLOCKS_SIZE);   
    }
    else{
        content_i = _AUX_intBinarySearch(
            folder_object->contentListBlocks, 
            CONTENT_LIST_BLOCKS_SIZE, 
            folder_first_block
        );
        
        if (content_i != -1){
            folder_object->contentListBlocks[content_i] = -1;
            _AUX_blockSort(folder_object->contentListBlocks, CONTENT_LIST_BLOCKS_SIZE);
        }
        else{
            int b_index = folder_object->nextBlockIndex;
            int next_index = _FAT_getNextBlock(FAT, b_index);
            while (b_index != -1){
                int* folder_block = (int*) _DISK_readBytes(DISK, b_index, BLOCK_SIZE);
                b_index = next_index;
                next_index = _FAT_getNextBlock(FAT, next_index);

                content_i = _AUX_intBinarySearch(
                    folder_block, 
                    128, 
                    folder_first_block
                );
                if (content_i == -1){
                    free(folder_block);
                    continue;
                }

                folder_block[content_i] = -1;
                _AUX_blockSort(folder_block, 128);
                _DISK_writeBytes(DISK, b_index, (char*) folder_block, BLOCK_SIZE);
                free(folder_block);
            }
        }   
    }

    folder_object->size--;
    folder_object->numFolders--;
    _DISK_writeBytes(DISK, CWD->firstBlockIndex, (char*) folder_object, sizeof(struct FolderObject));

    free(folder_object);
    return 0;

}






int _FILE_changeWorkingDirectory(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* new_WD_name){

    int b_index;

    if (strncmp(new_WD_name, "..", MAX_FILENAME_LEN) == 0){
        if (CWD->previousFolderBlockIndex == -1){
            printf("%s folder has no parent folder\n",CWD->folderName);
            return -1;
        }
        b_index = CWD->previousFolderBlockIndex;
    }
    else b_index = CWD->folderList[_FILE_searchFolderInCWD(CWD, new_WD_name)]->firstBlockIndex;
    

    int next_index = _FAT_getNextBlock(FAT, b_index);

    int n_folders = CWD->numFolders;
    int n_files = CWD->numFiles;

    FolderObject* new_CWD = (FolderObject*) _DISK_readBytes(
        DISK, 
        b_index, 
        sizeof(struct FolderObject)
    );

    // You can never be too much careful
    //new_CWD->previousFolderBlockIndex = CWD->firstBlockIndex;

    // Swapping CWD fields
    strncpy(CWD->folderName, new_CWD->folderName, MAX_FILENAME_LEN);
    CWD->firstBlockIndex = b_index;

    while (b_index != -1){
        CWD->currentBlockIndex = b_index;
        b_index = next_index;
        next_index = _FAT_getNextBlock(FAT, b_index);
    }

    CWD->size = new_CWD->size;
    CWD->previousFolderBlockIndex = new_CWD->previousFolderBlockIndex;

    // Freeing all previously allocated struct pointers
    for (int i=0; i<n_folders; ++i){
        free(CWD->folderList[i]);
    }
    for (int i=0; i<n_files; ++i){
        free(CWD->fileList[i]);
    }
    free(CWD->folderList);
    free(CWD->fileList);

    CWD->numFolders = new_CWD->numFolders;
    CWD->folderList = _FILE_getContainedFolders(DISK, FAT, new_CWD);
    CWD->numFiles = new_CWD->numFiles;
    CWD->fileList = _FILE_getContainedFiles(DISK, FAT, new_CWD);

    free(new_CWD);
    return 0;

}









int _FILE_getFileContent(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* file_name, char* dest_buffer){
    
    int cwd_index = _FILE_searchFileInCWD(CWD, file_name);
    if (CWD->fileList[cwd_index]->size == 0) return -1;

    int b_index = CWD->fileList[cwd_index]->firstBlockIndex;
    int next_index = _FAT_getNextBlock(FAT, b_index);

    int n_bytes = CWD->fileList[cwd_index]->size;
    int read_bytes = 0;
    
    while(b_index != -1){

        // I am reading the first file block
        if (read_bytes == 0){
            FileObject* file = (FileObject*) _DISK_readBytes(DISK, b_index, sizeof(struct FileObject));
            strncpy(dest_buffer, file->firstDataBlock, ( n_bytes < FIRST_DATA_BLOCK_SIZE ? n_bytes : FIRST_DATA_BLOCK_SIZE ));
            read_bytes = strlen(dest_buffer);
            free(file);
        }

        // I am reading the next file block(s)
        else{
            char* data_block = _DISK_readBytes(DISK, b_index, BLOCK_SIZE);
            strncpy(dest_buffer+read_bytes, data_block, ( n_bytes-read_bytes < BLOCK_SIZE ? n_bytes-read_bytes : BLOCK_SIZE ));
            read_bytes = strlen(dest_buffer);
            free(data_block);
        }

        b_index = next_index;
        next_index = _FAT_getNextBlock(FAT, b_index);
    }

    return 0;
}






int _FILE_writeFileContent(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* file_name, char* src_buffer, int n_bytes){

    int extra_blocks_to_write;
    if (n_bytes <= FIRST_DATA_BLOCK_SIZE) extra_blocks_to_write = 0;
    else extra_blocks_to_write = ((n_bytes-FIRST_DATA_BLOCK_SIZE-1) / BLOCK_SIZE) +1;

    int cwd_index = _FILE_searchFileInCWD(CWD, file_name);
    FileObject* file = (FileObject*) _DISK_readBytes(
        DISK, 
        CWD->fileList[cwd_index]->firstBlockIndex, 
        sizeof(struct FileObject)
    );

    int b_index = CWD->fileList[cwd_index]->firstBlockIndex;
    int next_index = _FAT_getNextBlock(FAT, b_index);

    strncpy(file->firstDataBlock, src_buffer, FIRST_DATA_BLOCK_SIZE);
    for (int i=0; i<extra_blocks_to_write; ++i){
        
        // I have to allocate a new block for the file
        if (next_index == -1){
            
            int new_block_index = _FAT_findFirstFreeBlock(FAT);
            if (new_block_index == -1){
                printf("[ERROR] No space left on DISK!\n");
                return -1;
            }

            if (_FAT_allocateBlock(FAT, new_block_index) != 0) return -1;
            _FAT_setNextBlock(FAT, b_index, new_block_index);

            if (_DISK_writeBytes(
                    DISK, 
                    new_block_index, 
                    src_buffer+(FIRST_DATA_BLOCK_SIZE + i*BLOCK_SIZE), 
                    BLOCK_SIZE
                ) == -1) return -1;

            next_index = new_block_index;
            if (i == 0) file->nextBlockIndex = new_block_index;

        }

        // There is already an allocated block for the file
        else{
            if (_DISK_writeBytes(
                    DISK, 
                    b_index, 
                    src_buffer+(FIRST_DATA_BLOCK_SIZE + i*BLOCK_SIZE),
                    BLOCK_SIZE
                ) == -1) return -1;
        }

        b_index = next_index;
        next_index = _FAT_getNextBlock(FAT, b_index);

    }

    // I have to deallocate any remaining emptied file blocks
    while (next_index != -1){
        b_index = next_index;
        next_index = _FAT_getNextBlock(FAT, b_index);
        _FAT_deallocateBlock(FAT, b_index);
    }
    if (extra_blocks_to_write == 0) file->nextBlockIndex = -1;


    file->size = n_bytes;
    CWD->fileList[cwd_index]->size = n_bytes;
    _DISK_writeBytes(
        DISK, 
        CWD->fileList[cwd_index]->firstBlockIndex, 
        (char*) file, 
        sizeof(struct FileObject)
    );

    free(file);
    return 0;

}










void _FILE_recursiveNameSearch(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* current_dir, char* name, char** found_paths, int* found, char* current_path, int chars_to_delete){

    // Max number of results reached
    if (*found >= 100) return;


    // I look for "name" in the current_dir

    int file_cwd_index = _FILE_searchFileInCWD(current_dir, name);
    if (file_cwd_index != -1){
        char* found_name = current_dir->fileList[file_cwd_index]->name;
        found_paths[*found] = calloc(strlen(current_path)+strlen(found_name), sizeof(char));

        strcpy(found_paths[*found], current_path);
        //strncpy(found_paths[*found], current_path, strlen(current_path));
        strcat(found_paths[*found], found_name);
        //strncpy(found_paths[*found]+strlen(found_paths[*found]), found_name, strlen(found_name));

        (*found)++;
    }

    // Repeated check after a possible allocation of found_paths
    if (*found >= 100) return;

    int folder_cwd_index = _FILE_searchFolderInCWD(current_dir, name);
    if (folder_cwd_index != -1){
        char* found_name = current_dir->folderList[folder_cwd_index]->name;
        found_paths[*found] = calloc(strlen(current_path)+strlen(found_name), sizeof(char));

        strcpy(found_paths[*found], current_path);
        //strncpy(found_paths[*found], current_path, strlen(current_path));
        strcat(found_paths[*found], found_name);
        //strncpy(found_paths[*found]+strlen(found_paths[*found]), found_name, strlen(found_name));
        strcat(found_paths[*found], "/");
        //strncpy(found_paths[*found]+strlen(found_paths[*found]), "/",2);

        (*found)++;
    }


    // Recursive path is over
    if (current_dir->numFolders == 0){
        
        //Cancello chars_to_delete da current_path (la profondità a cui sono arrivato)
        char s[strlen(current_path)-chars_to_delete];
        strncpy(s, current_path, strlen(current_path)-chars_to_delete);
        strcpy(current_path, s);

        return;
    }


    // Recursive steps
    for (int i=0; i<current_dir->numFolders; ++i){

        FolderObject* next_dir_obj = (FolderObject*) _DISK_readBytes(
            DISK, 
            current_dir->folderList[i]->firstBlockIndex, 
            sizeof(struct FolderObject)
        );

        // Path too long, folder ignored
        if ((strlen(current_path)+strlen(next_dir_obj->folderName)+1) > 1024){
            free(next_dir_obj);
            continue;
        }



        FolderHandle* next_dir_handle = calloc(1,sizeof(struct FolderHandle));
        strncpy(next_dir_handle->folderName, next_dir_obj->folderName, MAX_FILENAME_LEN);
        next_dir_handle->numFiles = next_dir_obj->numFiles;
        next_dir_handle->fileList = _FILE_getContainedFiles(DISK, FAT, next_dir_obj);
        next_dir_handle->numFolders = next_dir_obj->numFolders;
        next_dir_handle->folderList = _FILE_getContainedFolders(DISK, FAT, next_dir_obj);

        free(next_dir_obj);

        strcat(current_path, next_dir_handle->folderName);
        strcat(current_path, "/");
        //strcpy(current_path+strlen(current_path), next_dir_handle->folderName);
        //strcpy(current_path+strlen(current_path), "/");

        // Actual recursive call
        _FILE_recursiveNameSearch(
            DISK, 
            FAT, 
            next_dir_handle, 
            name, 
            found_paths, 
            found, 
            current_path,
            (chars_to_delete + strlen(next_dir_handle->folderName) +1)
        );        

        free(next_dir_handle);

    }

}


int _FILE_findPathsTo(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* name, char** found_paths){

    char current_path[1024];
    strcpy(current_path, CWD->folderName);
    strcat(current_path, "/");

    int found = 0;
    
    _FILE_recursiveNameSearch(DISK, FAT, CWD, name, found_paths, &found, current_path, 0);

    return found;

}