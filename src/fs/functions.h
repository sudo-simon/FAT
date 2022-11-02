#pragma once
#include "disk.h"
#include "fat.h"
#include "file.h"

//TODO: i parametri delle funzioni andranno probabilmente cambiati in strutture dati inizializzate
//TODO: dai wrapper in commands.c

int _FS_createFile(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* file_name);

int _FS_eraseFile(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* file_name);

int _FS_write(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* file_name, char* src_buffer);

int _FS_read(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* file_name, char* dest_buffer);

int _FS_seek(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* name, char** found_paths);

int _FS_createDir(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* dir_name);

int _FS_eraseDir(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* dir_name);

int _FS_changeDir(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* dir_name);

int _FS_listDir(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD);