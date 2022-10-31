#pragma once
#include "disk.h"
#include "fat.h"
#include "file.h"

//TODO: i parametri delle funzioni andranno probabilmente cambiati in strutture dati inizializzate
//TODO: dai wrapper in commands.c

int _FS_createFile(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* file_name);

int _FS_eraseFile(char* file_name);

int _FS_write();

int _FS_read();

int _FS_seek(char* file_name);  //TODO: find??

int _FS_createDir(char* dir_name);

int _FS_eraseDir(char* dir_name);

int _FS_changeDir(char* dir_name);

int _FS_listDir(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD);