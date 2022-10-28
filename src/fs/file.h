#pragma once
#include "../constants.h"
#include "disk.h"
#include "fat.h"


/*
File object on DISK
*/
typedef struct FileObject {
    char fileName[MAX_FILENAME_LEN];    // Name of the file, limited to MAX_FILENAME_LEN characters
    char isFolder;                      // Flag that differentiates Folders from Files
    struct FileObject** folderFiles;    // List of files (and folders) contained in the folder, NULL if isFolder == 0
    int firstDiskBlockIndex;            // Index of the first block of the file on disk
    int size;                           // Size of file in bytes or in # of files contained in the folder (if isFolder)
    char* data;                         // Actual data of the file
} FileObject;


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
Initializes the root folder at the start of the disk
*/
FileHandle* _FILE_initRoot(DISK_STRUCT* DISK, FAT_STRUCT* FAT);


/*
Returns a FileHandle for the root folder of the opened DISK
*/
FileHandle* _FILE_getRoot(DISK_STRUCT* DISK, FAT_STRUCT* FAT);


/*
Creates a FileObject on DISK.
Returns a pointer to the created FileObject
*/
FileHandle* _FILE_createFile(char* file_name);


/*
Procedurally removes the file from the FAT allocation.
Returns number of freed blocks
*/
int _FILE_deleteFile(int starting_block_index);


/*
Creates a folder (special type of FileObject) on DISK.
Returns a pointer to the created FileObject
*/
FileHandle* _FILE_createFolder(char* folder_name);


/*
Procedurally removes the folder from the FAT allocation, and all the files inside of it.
Returns the number of freed blocks
*/
int _FILE_deleteFolder(int starting_block_index);




