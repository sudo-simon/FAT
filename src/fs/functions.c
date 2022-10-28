// My headers
#include "functions.h"
#include "fat.h"
#include "file.h"

extern FAT_STRUCT* FAT;
extern char* DISK;


int _fs_createFile(char* file_name);

int _fs_eraseFile(char* file_name);

int _fs_write();

int _fs_read();

int _fs_seek(char* file_name);  //TODO: find??

int _fs_createDir(char* dir_name);

int _fs_eraseDir(char* dir_name);

int _fs_changeDir(char* dir_name);

int _fs_listDir(char* dir_name);