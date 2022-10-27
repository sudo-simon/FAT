#pragma once
#include "../constants.h"

/*
File image on DISK
*/
typedef struct FileImage {
    char fileName[MAX_FILENAME_LEN];    // Name of the file, limited to MAX_FILENAME_LEN characters
    char isFolder;                      // Flag that differentiates Folders from Files
    int firstDiskBlockIndex;            // Index of the first block of the file on disk
    int size;                           // Size of file in bytes or in # of files contained in the folder (if isFolder)
    char* data;                         // Actual data of the file
} FileImage;


/*
FileHandle struct returned to the user editing a file
*/
typedef struct FileHandle {
    char fileName[MAX_FILENAME_LEN];    // Name of the file, limited to MAX_FILENAME_LEN characters
    int firstDiskBlockIndex;            // Index of the first block of the file on disk
    int currentDiskBlockIndex;          // Index of the file block the user is currently working on (r/w)
    int size;                           // Size of file in bytes or in # of files contained in the folder (if isFolder)
} FileHandle;


/*

*/
int _FILE_createFile();


/*

*/
int _FILE_createFolder();


/*
Procedurally removes the file from the FAT allocation, returns number of freed blocks
*/
int _FILE_deleteFile(int starting_block_index);