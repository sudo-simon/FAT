#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// Readline library (libreadline-dev) 
#include <readline/readline.h>
#include <readline/history.h>
// My headers
#include "../constants.h"
#include "commands.h"
#include "shell.h"

extern char* USER;

int shell_init(){   

    char* time = current_time_str();
    clear_all();
    printf("---------- FAT - Fake Awesome Terminal ----------\n\n");
    printf("%s\n",time);
    free(time);
    return 0;

}


char* current_time_str(){

    char* str_time = malloc(32*sizeof(char));
    struct tm* current_time = malloc(sizeof(struct tm));
    time_t t = time(NULL);
    current_time = localtime(&t);

    char* day = malloc(3); char* mon = malloc(3); char* year = malloc(5);
    char* hour = malloc(3); char* min = malloc(3); char* sec = malloc(3);
    sprintf(day, "%d",current_time->tm_mday);
    sprintf(mon, "%d",(current_time->tm_mon)+1);
    sprintf(year, "%d",(current_time->tm_year)+1900);
    sprintf(hour, "%d",current_time->tm_hour);
    sprintf(min, "%d",current_time->tm_min);
    sprintf(sec, "%d",current_time->tm_sec);

    // correct date formatting (2 digits per value)
    if (strlen(day) < 2){
        char* tmp = malloc(3);
        strncpy(tmp, "0", 3);
        strncat(tmp, day, 3);
        strncpy(day, tmp, 3);
        free(tmp);
    }
    if (strlen(mon) < 2){
        char* tmp = malloc(3);
        strncpy(tmp, "0", 3);
        strncat(tmp, mon, 3);
        strncpy(mon, tmp, 3);
        free(tmp);
    }
    if (strlen(hour) < 2){
        char* tmp = malloc(3);
        strncpy(tmp, "0", 3);
        strncat(tmp, hour, 3);
        strncpy(hour, tmp, 3);
        free(tmp);
    }
    if (strlen(min) < 2){
        char* tmp = malloc(3);
        strncpy(tmp, "0", 3);
        strncat(tmp, min, 3);
        strncpy(min, tmp, 3);
        free(tmp);
    }
    if (strlen(sec) < 2){
        char* tmp = malloc(3);
        strncpy(tmp, "0", 3);
        strncat(tmp, sec, 3);
        strncpy(sec, tmp, 3);
        free(tmp);
    }

    sprintf(str_time, "    %s/%s/%s\n    %s:%s:%s", day, mon, year, hour, min, sec);
    free(mon); free(day); free(year); free(hour); free(min); free(sec);
    return str_time;

}


int take_input(char *input_buf, char* input_msg){

    char* buf;
    buf = readline(input_msg);
    int input_length = strlen(buf);
    if (input_length > 0){
        add_history(buf);
        strncpy(input_buf, buf, MAX_INPUT_LEN);
        free(buf);
        return 0;
    }
    else if (input_length > MAX_INPUT_LEN){
        printf("[ERROR] Input can be a maximum of %d characters long\n",MAX_INPUT_LEN);
        free(buf);
        return 1;
    }
    else{
        free(buf);
        return -1;
    }

}



int str_split(char *in, char** out){

    int n_str = 0;
    char* token = strtok(in, " ");

    while (token != NULL){
        if (n_str == 0) strncpy(out[0], token, MAX_INPUT_LEN);
        if (n_str == 1) strncpy(out[1], token, MAX_INPUT_LEN);
        token = strtok(NULL, " ");
        n_str++;
    }

    return n_str-1;

}





/* OLD FUNCTION
int str_split(char *in, char** out){

    int n = 0, current_word_index = 0;
    char c = '\0', prev_c = '\0';
    //boolean used to check if quotes are being used to define a single word
    char in_quotes = 0;

    for(int i=0; i<strlen(in); ++i){
        if (n < 2){
            c = in[i];
            if (c == '\'' || c == '\"'){
                //bitwise xor to swap between 0 and 1
                in_quotes = in_quotes^0x1;
                continue;
            }
            if(in_quotes)
                out[n][current_word_index++] = c;
            else{
                if (c != ' '){
                    out[n][current_word_index++] = c;
                }
                else{
                    if (prev_c != ' '){
                        ++n;
                        current_word_index = 0;
                    }
                }
            }
            prev_c = c;
        }
        else break;
    }
    if(in_quotes){
        printf("Error: bad command formatting, odd number of quotes\n");
        return -1;
    }
    return n;

}
*/



