#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
// My headers
#include "constants.h"
#include "cli/shell.h"
#include "cli/commands.h"


// 8 MiB mmapped disk
char DISK[8388608];


int _open_session(char* session_name){
    //TODO
    printf("Open session called, yet to be implemented");
    return 0;
}


int main(int argc, char** argv){

    //TODO: possibilità di passare un parametro: nome del file su cui è stata salvata una sessione FAT
    //TODO: oppure una stringa speciale (es. current_session) per aprire un nuovo terminale sulla
    //TODO: sessione corrente che lavora in simultanea (threading)

    // Argument passing, either the name of a session file or "_current_session_"
    char* new_session = NULL;
    if (argc > 1){
        new_session = argv[1];
        //TODO: sostituire con i signal (SIGUSR?)
        if (strcmp(new_session, "_fork_test_") == 0){
            __pid_t child_pid = fork();
            if (child_pid)
                printf("I am your father.\n");
            else{
                printf("Noooooooooooooooooo\n");
                exit(0);
            }
            return 69;
        }
        _open_session(new_session);
    }


    // Input strings buffers
    char* input_msg = malloc(128*sizeof(char));
    sprintf(input_msg, "\n%s >>> ",getenv("USER"));

    char* input = malloc(MAX_INPUT_LEN*sizeof(char));
    char** split_input = malloc(2*sizeof(char*));
    split_input[0] = malloc(MAX_INPUT_LEN*sizeof(char));
    split_input[1] = malloc(MAX_INPUT_LEN*sizeof(char));
    int n_args;
    short cmd_index;

    shell_init();
    //DEBUG
    if(new_session) printf("\n\nArgument passed! arg = %s",new_session);
    
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
                    free(input);
                    free(split_input[0]);
                    free(split_input[1]);
                    free(split_input);
                }
                else{
                    printf("quit doesn't take any arguments");
                    continue;
                }
            }
            int cmd_ret_value = (*FN_ARRAY[cmd_index])((void*)split_input[1]);
        }
        // Invalid command
        else{
            printf("%s is not a valid command",split_input[0]);
        }

    }

    return 0;
    
}