#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// My headers
#include "disk.h"
#include "file.h"
#include "functions.h"


int _FS_createFile(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* file_name){
    if (_FILE_existingFileName(CWD, file_name)){
        printf("There is already a file named %s in this folder\n",file_name);
        return -1;
    }
    return _FILE_createFile(DISK, FAT, CWD, file_name);
}


int _FS_eraseFile(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* file_name){
    if (! _FILE_existingFileName(CWD, file_name)){
        printf("There is no file named %s in this folder\n",file_name);
        return -1;
    }
    return _FILE_deleteFile(DISK, FAT, CWD, file_name);
}


int _FS_write();


int _FS_read(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* file_name, char* dest_buffer){
    if (! _FILE_existingFileName(CWD, file_name)){
        printf("There is no file named %s in this folder\n",file_name);
        return -1;
    }
    dest_buffer = _FILE_getFileContent(DISK, FAT, CWD, file_name);
    if (dest_buffer == NULL){
        printf("%s is empty\n",file_name);
        return -1;
    }
    return 0;
}


int _FS_seek(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* file_name, char* found_path);


int _FS_createDir(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* dir_name){
    if (_FILE_existingFolderName(CWD, dir_name)){
        printf("There is already a folder named %s in this folder\n",dir_name);
        return -1;
    }
    return _FILE_createFolder(DISK, FAT, CWD, dir_name);
}


int _FS_eraseDir(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* dir_name){
    if (! _FILE_existingFolderName(CWD, dir_name)){
        printf("There is no folder named %s in this folder\n",dir_name);
        return -1;
    }
    return _FILE_deleteFolder(DISK, FAT, CWD, dir_name, 0);
}


int _FS_changeDir(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* dir_name);


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