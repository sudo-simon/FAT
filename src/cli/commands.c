
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
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

//TODO: input string sanification!!

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
    if (strlen(new_filename) == 0){
        printf("File name can't be empty\n");
        return -1;
    }
    else if (strlen(new_filename) > MAX_FILENAME_LEN){
        printf("File name too long! It can be a maximum of %d characters long\n",MAX_FILENAME_LEN);
        return -1;
    }

    if(_FS_createFile(DISK, FAT, CWD, new_filename) == -1){
        printf("[ERROR] Unable to create the file %s\n",new_filename);
        return -1;
    }

    return 0;
}


int _rm(void *arg){
    char* file_name = (char*) arg;
    if (strlen(file_name) == 0){
        printf("Name of the file to delete is needed\n");
        return -1;
    }
    else if (strlen(file_name) > MAX_FILENAME_LEN){
        printf("File name too long! It can be a maximum of %d characters long\n",MAX_FILENAME_LEN);
        return -1;
    }

    if (_FS_eraseFile(DISK, FAT, CWD, file_name) == -1){
        printf("[ERROR] Unable to delete the file %s\n",file_name);
        return -1;
    }

    return 0;
}


int _cat(void* arg){
    char* file_name = (char*) arg;
    if (strlen(file_name) == 0){
        printf("Name of the file to read is needed\n");
        return -1;
    }
    else if (strlen(file_name) > MAX_FILENAME_LEN){
        printf("File name too long! It can be a maximum of 32 characters long\n");
        return -1;
    }

    char* output;
    if (_FS_read(DISK, FAT, CWD, file_name, output) == -1){
        printf("[ERROR] Unable to read the file %s\n",file_name);
        return -1;
    }
    printf("\n%s\n",output);
    
    free(output);
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
    char* new_foldername = (char*) arg;
    if (strlen(new_foldername) == 0){
        printf("Folder name can't be empty\n");
        return -1;
    }
    else if (strlen(new_foldername) > MAX_FILENAME_LEN){
        printf("Folder name too long! It can be a maximum of %d characters long\n",MAX_FILENAME_LEN);
        return -1;
    }

    if(_FS_createDir(DISK, FAT, CWD, new_foldername) == -1){
        printf("[ERROR] Unable to create the folder %s\n",new_foldername);
        return -1;
    }

    return 0;
}


int _rmdir(void *arg){
    char* folder_name = (char*) arg;
    if (strlen(folder_name) == 0){
        printf("Name of the folder to delete is needed\n");
        return -1;
    }
    else if (strlen(folder_name) > MAX_FILENAME_LEN){
        printf("Folder name too long! It can be a maximum of %d characters long\n",MAX_FILENAME_LEN);
        return -1;
    }

    if (_FS_eraseDir(DISK, FAT, CWD, folder_name) == -1){
        printf("[ERROR] Unable to delete the folder %s\n",folder_name);
        return -1;
    }

    return 0;
}


int _cd(void *arg){
    printf("changeDir not yet implemented");
    return 0;
}


int _ls(void *arg){
    if (strlen((char*)arg) > 0){
        printf("ls doesn't take any arguments\n");
        return -1;
    }

    _FS_listDir(DISK, FAT, CWD);
    return 0;
}


int _edit(void *arg){
    printf("editFile not yet implemented");
    return 0;
}


int _save(void* arg){
    if (strlen((char*)arg) > 0){
        printf("Save doesn't take any arguments\n");
        return -1;
    }

    if (DISK->mmappedFlag){
        FILE* f = fopen(DISK->sessionFileName, "w");
        if (fwrite(DISK->disk, 1, DISK_SIZE, f) == 0){
            printf("[ERROR] Unable to save DISK file locally\n");
            return -1;
        };
        printf("[DONE] FAT file saved successfully\n");
        fclose(f);
    }
    else{
        char session_name[1024];
        printf("FAT file name to be created: ");
        scanf("%s",session_name);

        if (strlen(session_name) <= 0){
            printf("FAT file name needed\n");
            return -1;
        }
        else if (strlen(session_name) > MAX_FILENAME_LEN){
            printf("FAT file name can be a maximum of %d characters long\n",MAX_FILENAME_LEN);
            return -1;
        }

        //TODO: input validation!!

        strncat(session_name, ".FAT", 4);

        if(access(session_name, F_OK) == 0){
            printf("[ERROR] The file %s already exists\n",session_name);
            return -1;
        }

        FILE* f = fopen(session_name,"w+");

        if (fwrite(DISK->disk, 1, DISK_SIZE, f) == 0){
            printf("[ERROR] Unable to save DISK file locally\n");
            return -1;
        };
        printf("[DONE] FAT file saved successfully\n");

        strncpy(DISK->sessionFileName, session_name, 64);
        DISK->mmappedFlag = 1;
        fclose(f);
    }
    
    return 0;
}