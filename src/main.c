#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
//#include <pthread.h>
//#include <signal.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
// My headers
#include "constants.h"
#include "cli/shell.h"
#include "cli/commands.h"
#include "fs/disk.h"
#include "fs/fat.h"
#include "fs/file.h"

char DEBUG_FLAG = 0;

// DISK_SIZE mmapped disk
//char* DISK;
//char MMAPPED_DISK_FLAG = 0;

// DISK struct
DISK_STRUCT* DISK;

// FAT struct
FAT_STRUCT* FAT;

// CWD (Current Working Directory)
FolderHandle* CWD;

// FileHandle of the currently opened file
FileHandle* O_FILE;


int main(int argc, char** argv){

    // ------------------------------------------------------------------------------------------ //
    // Argument passing and initializations (DISK, FAT, CWD)

    //TODO: possibilità di passare un parametro: nome del file su cui è stata salvata una sessione FAT
    //TODO: oppure una stringa speciale (es. current_session) per aprire un nuovo terminale sulla
    //TODO: sessione corrente che lavora in simultanea (threading)

    // Argument passing, either the name of a session file or "_fork_test_ (DEBUGGING)"

    char* session_filename = NULL;
    if (argc > 1){
        session_filename = argv[1];

        //TODO: sostituire con i signal (SIGUSR?)
        if (strcmp(session_filename, "_fork_test_") == 0){
            __pid_t child_pid = fork();
            if (child_pid){
                system("x-terminal-emulator -e \"echo 'Process, I am your father.' && sleep 3\"");

            }
            else{
                system("x-terminal-emulator -e \"echo 'Noooooooooooooooooo' && sleep 6\"");
                exit(0);
            }
        }

    }
    

    // DISK initialization with error checks
    DISK = calloc(1,sizeof(DISK_STRUCT));
    if (_DISK_init(DISK, session_filename)){
        printf("[ERROR] %s is not a valid session file!",session_filename);
        exit(-1);
        return 0;
    }

    // FAT initialization
    FAT = calloc(1,sizeof(FAT_STRUCT));
    int root_block_index = _FAT_init(FAT, DISK, DISK->mmappedFlag);

    // CWD initialization
    if (! DISK->mmappedFlag)
        CWD = _FILE_initRoot(DISK, FAT);
    else
        CWD = _FILE_getRoot(DISK, FAT);





    // ------------------------------------------------------------------------------------------ //
    // Main cycle


    // Input strings buffers
    char* input_msg = malloc(128*sizeof(char));
    sprintf(input_msg, "\n%s >>> ",getenv("USER"));

    char* input = malloc(MAX_INPUT_LEN*sizeof(char));
    char** split_input = malloc(2*sizeof(char*));
    split_input[0] = malloc(MAX_INPUT_LEN*sizeof(char));
    split_input[1] = malloc(MAX_INPUT_LEN*sizeof(char));
    int n_args;
    short cmd_index;
    int cmd_ret_value;

    shell_init();
    //DEBUG CODE
    if(session_filename) printf("\n\nArgument passed! arg = %s\n",session_filename);
    if(DEBUG_FLAG) printf("File passed as argument does not exist");
    
    // Main loop
    while(1){

        // Buffers reset

        strncpy(input, "", MAX_INPUT_LEN);
        strncpy(split_input[0], "", MAX_INPUT_LEN);
        strncpy(split_input[1], "", MAX_INPUT_LEN);
        cmd_index = -1;

        take_input(input,input_msg);
        n_args = str_split(input, split_input);

        if (n_args > 1){
            printf("Too many command arguments (max 1)");
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
                    free(input_msg);
                    free(input);
                    free(split_input[0]);
                    free(split_input[1]);
                    free(split_input);

                    // DEBUG CODE
                    /*printf(
                        "DEBUG:\nDISK->disk = %ld \nDISK->mmappedFlag = %d \nDISK->sessionFd = %d\n\n",
                        DISK->disk,
                        DISK->mmappedFlag,
                        DISK->sessionFd
                    );*/

                    if (_DISK_destroy(DISK)){
                        printf("[ERROR] Unable to munmap DISK!\n");
                        continue;
                    }
                    _FAT_destroy(FAT);
                }
                else{
                    printf("quit doesn't take any arguments");
                    continue;
                }
            }

            // Effective command call from function pointers array 
            cmd_ret_value = (*FN_ARRAY[cmd_index])((void*)split_input[1]);
        
        }
        // Invalid command
        else{
            printf("%s is not a valid command",split_input[0]);
        }

    }

    return 0;
    
}