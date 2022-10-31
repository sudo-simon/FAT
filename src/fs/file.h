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
    char isFolder;                              // Flag set to 1 if the Object is a FolderObject
    char folderName[MAX_FILENAME_LEN];          // Name of the folder, limited to MAX_FILENAME_LEN characters
    int nextBlockIndex;                         // Index of the next block of the folder on disk
    int size;                                   // Number of files contained in the folder
    int numFolders;
    int numFiles;
    int previousFolderBlockIndex;               // Index of the block on DISK of the previous folder in the directory tree
    int contentListBlocks[(BLOCK_SIZE-56)/4];   // Indexes of the blocks containing the first 117 contents of the folder
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
    int previousFolderBlockIndex;           // Index of the block on DISK of the previous folder in the directory tree
    int numFolders;                         // Number of folders contained in the folder
    struct FolderListElem** folderList;      
    int numFiles;                           // Number of files contained in the folder
    struct FolderListElem** fileList;  
} FolderHandle;



/*
FolderListElem struct to be contained in the FolderHandle struct
*/
typedef struct FolderListElem {
    char name[MAX_FILENAME_LEN];
    int firstBlockIndex;
    int size;
} FolderListElem;


/*
Initializes the root folder at the start of the disk
*/
FolderHandle* _FILE_initRoot(DISK_STRUCT* DISK, FAT_STRUCT* FAT);


/*
Returns a FileHandle for the root folder of the opened DISK
*/
FolderHandle* _FILE_getRoot(DISK_STRUCT* DISK, FAT_STRUCT* FAT);


/*
Deallocates a FolderHandle from memory
*/
void _FILE_folderHandleDestroy(FolderHandle* folder_handle);


/*
Deallocates a FileHandle from memory
*/
void _FILE_fileHandleDestroy(FileHandle* file_handle);


/*
Puts a list of pointers to FolderObject in folder_list.
Returns the number of folder pointers now present in folder_list.
THE FUNCTION ALLOCATES THE FOLDER_LIST POINTER
*/
int _FILE_getContainedFolders(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderObject* folder, FolderListElem** folder_block_list);


/*
Puts a list of pointers to FileObject in file_list.
Returns the number of files pointers now present in file_list.
THE FUNCTION ALLOCATES THE FILE_LIST POINTER
*/
int _FILE_getContainedFiles(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderObject* folder, FolderListElem** file_block_list);


/*
Checks if a file with the same name exists in the CWD.
Returns 0 if false, 1 if true
*/
char _FILE_existingFileName(FolderHandle* CWD, char* file_name);


/*
Checks if a folder with the same name exists in the CWD.
Returns 0 if false, 1 if true
*/
char _FILE_existingFolderName(FolderHandle* CWD, char* folder_name);


/*
Creates a FileObject on DISK.
Returns the block index where the first block of the file is located on DISK
*/
int _FILE_createFile(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* file_name);


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


/*
Adds a file to the directory tree, both in CWD and on DISK
*/
int _FILE_folderAddFile(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, FileObject* new_file, int file_first_block);


/*

*/
int _FILE_folderRemoveFile(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, int file_first_block);


/*

*/
int _FILE_folderAddFolder(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, FolderObject* new_folder, int folder_first_block);


/*

*/
int _FILE_folderRemoveFolder(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, int folder_first_block);


/*

*/
FolderHandle* _FILE_changeWorkingDirectory(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* new_WD_name);



