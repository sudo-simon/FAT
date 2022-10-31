#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// My headers
#include "disk.h"
#include "functions.h"


int _FS_createFile(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* file_name){
    if (_FILE_existingFileName(CWD, file_name)){
        printf("There is already a file named %s in this folder\n",file_name);
        return -1;
    }
    return _FILE_createFile(DISK, FAT, CWD, file_name);
}


int _FS_eraseFile(char* file_name);


int _FS_write();


int _FS_read();


int _FS_seek(char* file_name);  //TODO: find??


int _FS_createDir(char* dir_name);


int _FS_eraseDir(char* dir_name);


int _FS_changeDir(char* dir_name);


int _FS_listDir(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD){
    
    printf("total %d\n",CWD->size);

    for (int i=0; i<CWD->numFolders; ++i){

        FolderListElem* folder = CWD->folderList[i];
        printf("[%d elems] %s/\n",folder->size, folder->name);

    }

    for (int i=0; i<CWD->numFiles; ++i){

        FolderListElem* file = CWD->fileList[i];
        printf("[%d bytes] %s\n",file->size, file->name);

    }

    return 0;

}