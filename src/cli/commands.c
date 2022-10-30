
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// My headers
#include "../constants.h"
#include "../cli/shell.h"
#include "../fs/functions.h"
#include "../fs/disk.h"
#include "../fs/fat.h"
#include "../fs/file.h"

extern DISK_STRUCT* DISK;
extern FAT_STRUCT* FAT;
extern FolderHandle* CWD;
extern FileHandle* O_FILE;


int _quit(void* arg){
    printf("Exiting FAT...\n");
    exit(0);
    return 0;
}


int _clear(void* args){
    return shell_init();
}


int _echo(void* string){
    return printf("%s",((char*)string));
}


int _mk(void *arg){
    char* new_filename = (char*) arg;
    if (strlen(new_filename) > MAX_FILENAME_LEN){
        printf("File name too long! Max 32 chars\n");
        return -1;
    }

    _FS_createFile(DISK, FAT, CWD, new_filename);

    return 0;
}


int _rm(void *arg){
    printf("eraseFile not yet implemented");
    return 0;
}


int _cat(void* arg){
    printf("cat not yet implemented");
    return 0;
}


int writeFile(void *arg){
    printf("writeFile not yet implemented");
    return 0;
}


int readFile(void *arg){
    printf("readFile not yet implemented");
    return 0;
}


int _find(void *arg){
    printf("seek not yet implemented");
    return 0;
}


int _mkdir(void *arg){
    printf("createDir not yet implemented");
    return 0;
}


int _rmdir(void *arg){
    printf("eraseDir not yet implemented");
    return 0;
}


int _cd(void *arg){
    printf("changeDir not yet implemented");
    return 0;
}


int _ls(void *arg){
    printf("listDir not yet implemented");
    return 0;
}


int _edit(void *arg){
    printf("editFile not yet implemented");
    return 0;
}
