#pragma once
#include "disk.h"
#include "fat.h"
#include "file.h"

void _AUX_alphabeticalSort(FolderListElem** elem_list, int list_len);

void _AUX_blockSort(int* block_list, int list_len);

int _AUX_intBinarySearch(int* list, int list_len,int to_find);

//int _AUX_fileBinarySearch(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* file_name);

//int _AUX_folderBinarySearch(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* folder_name);

int _AUX_validateInput(char* input_str);