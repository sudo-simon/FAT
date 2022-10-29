#pragma once
#include "../constants.h"
#include "disk.h"
#include "fat.h"


/*
File object on DISK.
Struct size = BLOCK_SIZE
*/
typedef struct FileObject {
    char isFolder;                          // Flag set to 1 if the Object is a FolderObject
    char fileName[MAX_FILENAME_LEN];        // Name of the file, limited to MAX_FILENAME_LEN characters
    int nextBlockIndex;                     // Index of the next block of the file on disk
    int size;                               // Size of file in bytes or in # of files contained in the folder (if isFolder)
    char firstDataBlock[BLOCK_SIZE-44];     // First (BLOCK_SIZE-44) bytes of the file
} FileObject;


/*
Folder Object on DISK.
Struct size = BLOCK_SIZE
*/
typedef struct FolderObject {
    char isFolder;                          // Flag set to 1 if the Object is a FolderObject
    char folderName[MAX_FILENAME_LEN];
    int nextBlockIndex;
    int size;
    int contentListBlocks[(BLOCK_SIZE-44)/4];
} FolderObject;


/*
FileHandle struct returned to the user editing a file
*/
typedef struct FileHandle {
    char fileName[MAX_FILENAME_LEN];        // Name of the file, limited to MAX_FILENAME_LEN characters
    int firstBlockIndex;                    // Index of the first block of the file on disk
    int currentBlockIndex;                  // Index of the file block the user is currently working on (r/w)
    int size;                               // Size of file in bytes or in # of files contained in the folder (if isFolder)
    char currentBlock[BLOCK_SIZE];          // Buffer containing the current opened disk block 
} FileHandle;


/*
FolderHandle struct returned to the user as the CWD
*/
typedef struct FolderHandle {
    char folderName[MAX_FILENAME_LEN];      // Name of the folder, limited to MAX_FILENAME_LEN characters
    int firstBlockIndex;                    // Index of the first block of the file (folder) on disk
    int currentBlockIndex;                  // Index of the folder block the user is currently working on (r/w)
    int size;                               // Number of files contained in the folder
    struct FolderObject** folderList;       // ist of folders contained in the folder
    struct FileObject** fileList;           // List of files contained in the folder
} FolderHandle;


/*
Initializes the root folder at the start of the disk
*/
FolderHandle* _FILE_initRoot(DISK_STRUCT* DISK, FAT_STRUCT* FAT);


/*
Returns a FileHandle for the root folder of the opened DISK
*/
FolderHandle* _FILE_getRoot(DISK_STRUCT* DISK, FAT_STRUCT* FAT);


/*

*/
void*** _FILE_getContentPointers(DISK_STRUCT* DISK, int* content_blocks, int folder_size);


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
FolderHandle* _FILE_createFolder(char* folder_name);


/*
Procedurally removes the folder from the FAT allocation, and all the files inside of it.
Returns the number of freed blocks
*/
int _FILE_deleteFolder(int starting_block_index);




