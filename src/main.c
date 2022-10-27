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
#include "fs/fat.h"

char DEBUG_FLAG = 0;

// DISK_SIZE mmapped disk
char* DISK;
char MMAPPED_DISK_FLAG = 0;

// FAT struct
FAT_STRUCT* FAT;


int _open_session(char* session_filename){

    // No previous session file opened
    if (!session_filename){
        DISK = calloc(DISK_SIZE,sizeof(char));
    }
    // Existing session file to be opened
    else{
        if(access(session_filename, F_OK)){
            printf("The file %s does not exist",session_filename);
            DEBUG_FLAG = 1;
            return -1;
        }

        int session_fd = open(
            session_filename,
            0,
            O_RDWR
        );

        DISK = mmap(
            NULL,
            DISK_SIZE,
            PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_POPULATE,
            session_fd,
            0
        );
        
        MMAPPED_DISK_FLAG = 1;
    }

    // Allocation of FAT
    FAT = calloc(1,sizeof(FAT_STRUCT));

    printf("Open session called, yet to be implemented\n");
    return 0;

}


int main(int argc, char** argv){

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
    _open_session(session_filename);
    int first_FAT_block = _FAT_init(MMAPPED_DISK_FLAG);


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
                    if (MMAPPED_DISK_FLAG){
                        if(munmap(DISK, DISK_SIZE)){
                            printf("ERROR: mmunmap(DISK) not succesful\n");
                            return -1;
                        }
                    }
                    else{
                        free(DISK);
                        free(FAT);
                    }
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