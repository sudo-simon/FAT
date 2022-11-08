#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
// My headers
#include "constants.h"
#include "cli/shell_linenoise.h"
#include "cli/commands.h"
#include "fs/disk.h"
#include "fs/fat.h"
#include "fs/file.h"

//char DEBUG_FLAG = 0;

// DISK struct
DISK_STRUCT* DISK;

// FAT struct
FAT_STRUCT* FAT;

// CWD (Current Working Directory)
FolderHandle* CWD;

// FileHandle of the currently opened file
////FileHandle* O_FILE;

// Flag to check if an editor is open
char EDITOR_OPEN = 0;


int main(int argc, char** argv){

    // ------------------------------------------------------------------------------------------ //
    // Argument passing and initializations (DISK, FAT, CWD, O_FILE)


    // Argument passing
    char* session_filename = NULL;
    if (argc > 1)
        session_filename = argv[1];

    // DISK initialization
    DISK = calloc(1,sizeof(DISK_STRUCT));
    if (_DISK_init(DISK, session_filename)){
        printf("[ERROR] %s is not a valid session file!\n",session_filename);
        exit(-1);
        return 0;
    }

    // FAT initialization
    FAT = calloc(1,sizeof(FAT_STRUCT));
    _FAT_init(FAT, DISK, DISK->persistentFlag);

    // CWD initialization
    if (! DISK->persistentFlag)
        CWD = _FILE_initRoot(DISK, FAT);
    else
        CWD = _FILE_getRoot(DISK, FAT);

    // O_FILE initialization
    ////O_FILE = calloc(1, sizeof(struct FileHandle));



    // ------------------------------------------------------------------------------------------ //
    // Main cycle

    
    int n_args;
    short cmd_index;

    char input_msg[128];
    char input[MAX_INPUT_LEN];
    char split_input_0[MAX_INPUT_LEN];
    char split_input_1[MAX_INPUT_LEN];
    char* split_input[2] = { split_input_0, split_input_1 };

    _SHELL_init();
    _SHELL_clear();


    /*/DEBUG CODE
    printf("diskpersistentFlag = %d\n",DISK->persistentFlag);
    printf("sessionFileName = %s\n",DISK->sessionFileName);
    printf("CWD->numFiles = %d\n",CWD->numFiles);
    printf("CWD->firstBlockIndex = %d\n",CWD->firstBlockIndex);
    printf("CWD->size = %d\n",CWD->size);
    printf("CWD->fileList[0]->name = %s\n",CWD->fileList[0]->name);
    */

    
    // -------------------------- MAIN LOOP --------------------------
    while(1){

        // Input buffers reset
        strncpy(input, "", MAX_INPUT_LEN);
        strncpy(split_input[0], "", MAX_INPUT_LEN);
        strncpy(split_input[1], "", MAX_INPUT_LEN);
        cmd_index = -1;

        sprintf(input_msg, "\n[%s] %s >>> ",getenv("USER"),CWD->folderName);

        // Linenoise input taking
        _SHELL_takeInput(input, input_msg);

        n_args = str_split(input,split_input);

        if (n_args > 1){
            printf("Too many command arguments (max 1)\n");
            continue;
        }

        // Searches the entered command in CMD_ARRAY (defined in commands.h)
        for(int i=0; i<TOTAL_COMMANDS; ++i){
            if(strncmp(split_input[0], CMD_ARRAY[i], MAX_INPUT_LEN) == 0){
                cmd_index = i;
                break;
            }
        }

        // Call to the right function pointer in FN_ARRAY (defined in commands.h)
        // Valid command
        if (cmd_index != -1){
            // if cmd == quit buffers get freed
            if (strcmp(split_input[0],"quit") == 0){
                if (n_args == 0){

                    // DEBUG CODE
                    /*printf(
                        "DEBUG:\nDISK->disk = %ld \nDISK->persistentFlag = %d \nDISK->sessionFd = %d\n\n",
                        DISK->disk,
                        DISK->persistentFlag,
                        DISK->sessionFd
                    );*/

                    // DISK, FAT, CWD and O_FILE deallocation
                    if (_DISK_destroy(DISK)){
                        printf("[ERROR] Unable to unmap DISK!\n");
                        continue;
                    }
                    _FAT_destroy(FAT);
                    _FILE_folderHandleDestroy(CWD);
                    ////_FILE_fileHandleDestroy(O_FILE);

                }
                else{
                    printf("quit doesn't take any arguments\n");
                    continue;
                }
            }

            if (EDITOR_OPEN){
                printf("Close the editor before performing any other operation\n");
                continue;
            }

            // Effective command call from function pointers array 
            (*FN_ARRAY[cmd_index])((void*)split_input[1]);
        
        }

        // Invalid command
        else 
            printf("%s is not a valid command\nUse command \"help\" for a list of valid commands\n",split_input[0]);

    }

    return 0;
    
}