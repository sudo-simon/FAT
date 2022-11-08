
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
// My headers
#include "../constants.h"
#include "../cli/shell_linenoise.h"
#include "../fs/aux.h"
#include "../fs/functions.h"
#include "../fs/disk.h"
#include "../fs/fat.h"
#include "../fs/file.h"
#include "../editor/kilo.h"

extern DISK_STRUCT* DISK;
extern FAT_STRUCT* FAT;
extern FolderHandle* CWD;
extern char EDITOR_OPEN;


int _help(void *arg){
    char help_msg[] = (
        "FAT commands:\n"
        "   quit: exits FAT\n"
        "   echo: prints argument to stdout\n"
        "   \n"
    );
    printf("%s\n",help_msg);
    return 0;
}


int _quit(void* arg){
    printf("Exiting FAT...\n");
    exit(0);
    return 0;
}


int _clear(void* args){
    _SHELL_clear();
    return 0;
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

    // Input validation for a file name
    if (! _AUX_validateInput(new_filename)){
        printf("[ERROR] Forbidden characters in file name ('/' ',' '\\')\n");
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
        printf("File name too long! It can be a maximum of %d characters long\n",MAX_FILENAME_LEN);
        return -1;
    }

    if (! _FILE_existingFileName(CWD, file_name)){
        printf("There is no file named %s in this folder\n",file_name);
        return -1;
    }

    char output[CWD->fileList[_FILE_searchFileInCWD(CWD, file_name)]->size];
    if (_FS_read(DISK, FAT, CWD, file_name, output) == -1){
        printf("[ERROR] Unable to read the file %s\n",file_name);
        return -1;
    }
    printf("\n%s\n",output);
    
    return 0;
}


int _find(void *arg){

    char* name = (char*) arg;
    if (strlen(name) == 0){
        printf("Name of the file to search is needed\n");
        return -1;
    }
    else if (strlen(name) > MAX_FILENAME_LEN){
        printf("File name too long! It can be a maximum of %d characters long\n",MAX_FILENAME_LEN);
        return -1;
    }

    // Input validation for a name
    if (! _AUX_validateInput(name)){
        printf("[ERROR] Forbidden characters in name to search ('/' ',' '\\')\n");
        return -1;
    }



    // Max 100 results returned
    char** found_paths = calloc(100, sizeof(char*));

    int found = _FS_seek(DISK, FAT, CWD, name, found_paths);
    if (found == 0){
        printf("%s not found\n",name);
    }
    else if (found > 0){
        printf("%s found!\n",name);
        for (int i=0; i<found; ++i)
            printf("Path: %s\n",found_paths[i]);
    }
    else{
        printf("[ERROR] Search of %s is not possible\n",name);
    }

    for (int i=0; i<found; ++i)
        free(found_paths[i]);
    free(found_paths);

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

    // Doesn't break anything, just ugly
    if (strcmp(new_foldername, "root") == 0){
        printf("You can't name a folder \"root\"\n");
        return -1;
    }

    // Input validation for a folder name
    if (! _AUX_validateInput(new_foldername)){
        printf("[ERROR] Forbidden characters in folder name ('/' ',' '\\')\n");
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
    char* folder_name = (char*) arg;
    if (strlen(folder_name) == 0){
        printf("Name of the folder to open is needed\n");
        return -1;
    }
    else if (strlen(folder_name) > MAX_FILENAME_LEN){
        printf("Folder name too long! It can be a maximum of %d characters long\n",MAX_FILENAME_LEN);
        return -1;
    }

    if (_FS_changeDir(DISK, FAT, CWD, folder_name) == -1){
        printf("[ERROR] Unable to open the folder %s\n",folder_name);
        return -1;
    }

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


/*/ Auxiliary function for multithreading
void* _aux_editorStart(void* arg){
    system("x-terminal-emulator -e './fat.bin '");
    return NULL;
}
*/



int _edit(void *arg){
    
    char* file_name = (char*) arg;
    if (strlen(file_name) == 0){
        printf("Name of the file to edit is needed\n");
        return -1;
    }
    else if (strlen(file_name) > MAX_FILENAME_LEN){
        printf("File name too long! It can be a maximum of %d characters long\n",MAX_FILENAME_LEN);
        return -1;
    }

    if (! _FILE_existingFileName(CWD, file_name)){
        printf("There is no file named %s in this folder\n",file_name);
        return -1;
    }

    // ----------------------- EDITOR START -----------------------------

    EDITOR_OPEN = 1;

    char* file_content;
    int file_len = CWD->fileList[_FILE_searchFileInCWD(CWD, file_name)]->size;

    if (file_len == 0){
        file_content = calloc(2,1);
        strcpy(file_content, "");
    }
    else{
        file_content = calloc(file_len, sizeof(char));
        if (_FILE_getFileContent(DISK, FAT, CWD, file_name, file_content) == -1){
            printf("[ERROR]: unable to read %s file",file_name);
            goto editor_end;
        }
    }
    

    // ----------------- PIPE CREATION -----------------
    int editor_pipe[2]; // [0] = r --- [1] = w
    if (pipe(editor_pipe) == -1){
        printf("[ERROR]: IPC handling\n");
        goto editor_end;
    }
    
    
    // ---------------------- FORK ---------------------
    pid_t editor_pid = fork();
    if (editor_pid == -1){
        printf("[ERROR]: fork error!\n");
        close(editor_pipe[1]);
        goto editor_end;
    }


    // KILO EDITOR PROCESS
    if (editor_pid == 0){
        close(editor_pipe[0]); // Closing read pipe
        _KILO_start(file_name, file_len, editor_pipe[1]);
        free(file_content);
        //exit(0);
        return 0;
    }

    // FAT MAIN PROCESS
    else{
        close(editor_pipe[1]);  // Closing write pipe

        int kilo_exit_status;
        wait(&kilo_exit_status);
        
        if (kilo_exit_status == -1){
            goto editor_end;
        }

        // First 4 bytes of the pipe are the new file length
        int new_len;
        if (read(editor_pipe[0], &new_len, sizeof(int)) == -1){
            printf("[ERROR] Unable to read form pipe!\n");
            goto editor_end;
        }

        // Rest of the pipe is the new file content
        char* saved_file = calloc(new_len, 1);
        if (read(editor_pipe[0], saved_file, new_len) == -1){
            printf("[ERROR] Unable to read form pipe!\n");
            free(saved_file);
            goto editor_end;
        }

        if (_FILE_writeFileContent(DISK, FAT, CWD, file_name, saved_file, new_len) == -1){
            printf("[ERROR] Unable to write on DISK!\n");
        }
        free(saved_file);

    }

    // ----------------------- EDITOR STOP -----------------------------


editor_end:
    close(editor_pipe[0]);
    _SHELL_clear();
    EDITOR_OPEN = 0;
    free(file_content);
    return 0;

}


int _save(void* arg){
    if (strlen((char*)arg) > 0){
        printf("save doesn't take any arguments\n");
        return -1;
    }

    // Writing FAT in the first 70 blocks of DISK
    if (_FAT_writeOnDisk(FAT, DISK) == -1){
        printf("[ERROR] Unable to save FAT on DISK\n");
        return -1;
    }

    if (DISK->persistentFlag){
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

        // Input validation for a file name
        if (! _AUX_validateInput(session_name)){
            printf("[ERROR] Forbidden characters in file name ('/' ',' '\\')\n");
            return -1;
        }

        // Adding the .FAT extension
        strcat(session_name, ".FAT");

        // Checking if file already exists
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
        DISK->persistentFlag = 1;
        fclose(f);
    }
    
    return 0;
}




//TEMPORARY FUNCTION
int _write(void* arg){
    char* file_name = (char*) arg;
    if (strlen(file_name) == 0){
        printf("Name of the file to edit is needed\n");
        return -1;
    }
    else if (strlen(file_name) > MAX_FILENAME_LEN){
        printf("File name too long! It can be a maximum of %d characters long\n",MAX_FILENAME_LEN);
        return -1;
    }

    if (! _FILE_existingFileName(CWD, file_name)){
        printf("There is no file named %s in this folder\n",file_name);
        return -1;
    }



    char buf[524288]; //512 KBit buffer
    char* s = buf;
    int c;

    printf("Write the new content of the file (press Enter twice to stop writing):\n\n");
    while( (c = fgetc(stdin)) != EOF && s < buf + sizeof(buf) - 1 ){
        if( c == '\n' && s > buf && s[-1] == '\n' ){
            ungetc(c, stdin);
            break;
        }
        *s++ = c;
    }
    *s = '\0';



    if (_FS_write(DISK, FAT, CWD, file_name, buf) == -1){
        printf("[ERROR] Unable to write the file %s\n",file_name);
        return -1;
    }
    
    return 0;
}