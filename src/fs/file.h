#pragma once
#include "../constants.h"
#include "disk.h"
#include "fat.h"

#define FIRST_DATA_BLOCK_SIZE 468
#define CONTENT_LIST_BLOCKS_SIZE 114

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
// The integer comparator of the block indexes in contentListBlocks
//int _FILE_contentListBlocks_comparator(const void* n1, const void* n2);


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
// The Alphabetical comparator of the FolderListElem struct
//int _FILE_FolderListElem_comparator(const void* n1, const void* n2);


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
Puts a list of pointers to FolderListElem in folder_list.
Returns the number of folder pointers now present in folder_list.
THE FUNCTION ALLOCATES THE FOLDER_LIST POINTER
*/
FolderListElem** _FILE_getContainedFolders(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderObject* folder);


/*
Puts a list of pointers to FolderListElem in file_list.
Returns the number of files pointers now present in file_list.
THE FUNCTION ALLOCATES THE FILE_LIST POINTER
*/
FolderListElem** _FILE_getContainedFiles(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderObject* folder);


/*
Binary search of folder named folder_name in the CWD.
Returns the index of the folder if it is found, -1 otherwise
*/
int _FILE_searchFolderInCWD(FolderHandle* CWD, char* folder_name);
/*
Binary search of file named file_name in the CWD.
Returns the index of the file if it is found, -1 otherwise
*/
int _FILE_searchFileInCWD(FolderHandle* CWD, char* file_name);


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
Returns 0 if successful, -1 otherwise
*/
int _FILE_deleteFile(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* file_name);


/*
Creates a FolderObject on DISK.
Returns the block index where the first block of the folder is located on DISK
*/
int _FILE_createFolder(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* folder_name);


/*
Procedurally removes the folder from the FAT allocation, and all the files inside of it.
Returns 0 if successful, -1 otherwise
*/
int _FILE_deleteFolder(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* folder_name, char recursive_flag);


/*
Recursive call to remove all files and folders contained in a folder
*/
void _FILE_recursiveFolderDelete(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* current_dir, FolderObject* folder_object);


/*
Adds a file to the directory tree, both in CWD and on DISK
*/
int _FILE_folderAddFile(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, FileObject* new_file, int file_first_block);


/*
Removes a file from the directory tree, both in CWD and on DISK
*/
int _FILE_folderRemoveFile(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, int file_first_block);


/*
Adds a folder to the directory tree, both in CWD and on DISK
*/
int _FILE_folderAddFolder(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, FolderObject* new_folder, int folder_first_block);


/*
Removes a folder from the directory tree, both in CWD and on DISK
*/
int _FILE_folderRemoveFolder(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, int folder_first_block);


/*
Sets the CWD to new_WD_name if it is a valid working directory
*/
int _FILE_changeWorkingDirectory(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* new_WD_name);


/*
Reads the content of a file and puts it in the dest_buffer parameter
*/
int _FILE_getFileContent(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* file_name, char* dest_buffer);


/*
Writes n_bytes of the content of src_buffer to the file specified with file_name
*/
int _FILE_writeFileContent(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* file_name, char* src_buffer, int n_bytes);



