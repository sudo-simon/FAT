#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// My headers
#include "constants.h"
#include "cli/shell.h"
#include "cli/commands.h"




int main(int argc, char** argv){

    // Input strings buffers
    char* input_msg = malloc(128*sizeof(char));
    sprintf(input_msg, "\n%s >>> ",getenv("USER"));

    char* input = malloc(MAX_INPUT_LEN*sizeof(char));
    char** split_input = malloc(2*sizeof(char*));
    split_input[0] = malloc(MAX_INPUT_LEN*sizeof(char));
    split_input[1] = malloc(MAX_INPUT_LEN*sizeof(char));
    short cmd_index;

    shell_init();

    // Main loop
    while(1){

        // Buffers reset

        strncpy(input, "", MAX_INPUT_LEN);
        strncpy(split_input[0], "", MAX_INPUT_LEN);
        strncpy(split_input[1], "", MAX_INPUT_LEN);
        cmd_index = -1;

        take_input(input,input_msg);

        int n_args = str_split(input, split_input);
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