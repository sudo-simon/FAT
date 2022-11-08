#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Linenoise library 
#include "../libs/linenoise/linenoise.h"

// My headers
#include "../constants.h"
#include "shell_linenoise.h"

extern char* USER;


char* current_time_str(){

    char* str_time = malloc(32*sizeof(char));
    struct tm* current_time;
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



int str_split(char* in, char** out){

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


void _SHELL_clear(){

    char* time = current_time_str();
    clear_all();
    printf("FAT - Fake Awesome Terminal\n\n");
    printf("%s\n",time);
    free(time);    

}


// ------------------------ LINENOISE IMPLEMENTATION -------------------------------------

void completion(const char *buf, linenoiseCompletions *lc) {
    switch (buf[0]) {
        case 'q':
            linenoiseAddCompletion(lc,"quit");
            break;
        case 'c':
            linenoiseAddCompletion(lc,"cat");
            linenoiseAddCompletion(lc,"cd");
            linenoiseAddCompletion(lc,"clear");
            break;
        case 'e':
            linenoiseAddCompletion(lc, "echo");
            linenoiseAddCompletion(lc, "edit");
            break;
        case 'm':
            linenoiseAddCompletion(lc, "mk");
            linenoiseAddCompletion(lc, "mkdir");
            break;
        case 'r':
            linenoiseAddCompletion(lc, "rm");
            linenoiseAddCompletion(lc, "rmdir");
            break;
        case 'f':
            linenoiseAddCompletion(lc, "find");
            break;
        case 'l':
            linenoiseAddCompletion(lc, "ls");
            break;
        case 's':
            linenoiseAddCompletion(lc, "save");
            break;
        case 'h':
            linenoiseAddCompletion(lc, "help");
            break;
        default:
            linenoiseAddCompletion(lc, "help");
            break;
    }
}

char *hints(const char *buf, int *color, int *bold) {
    if (!strcmp(buf,"q")) { *color = 32; *bold = 0; return "uit"; }

    if (!strcmp(buf,"c")) { *color = 32; *bold = 0; return "d"; }
    if (!strcmp(buf,"ca")) { *color = 32; *bold = 0; return "t"; }
    if (!strcmp(buf,"cl")) { *color = 32; *bold = 0; return "ear"; }

    if (!strcmp(buf,"e")) { *color = 32; *bold = 0; return "cho"; }
    if (!strcmp(buf,"ed")) { *color = 32; *bold = 0; return "it"; }

    if (!strcmp(buf,"m")) { *color = 32; *bold = 0; return "k"; }
    if (!strcmp(buf,"mk")) { *color = 32; *bold = 0; return "dir"; }

    if (!strcmp(buf,"r")) { *color = 32; *bold = 0; return "m"; }
    if (!strcmp(buf,"rm")) { *color = 32; *bold = 0; return "dir"; }

    if (!strcmp(buf,"f")) { *color = 32; *bold = 0; return "ind"; }

    if (!strcmp(buf,"l")) { *color = 32; *bold = 0; return "s"; }

    if (!strcmp(buf,"s")) { *color = 32; *bold = 0; return "ave"; }

    if (!strcmp(buf,"h")) { *color = 32; *bold = 0; return "elp"; }

    return NULL;
}

void hint_free(void* buf){ free(buf); }




void _SHELL_init(){
    linenoiseSetMultiLine(0);

    //linenoiseSetCompletionCallback(completion);   // Bad behaviour
    //linenoiseSetHintsCallback(hints);

    linenoiseHistoryLoad("linenoise_history.txt");
}


void _SHELL_takeInput(char* input_buf, char* input_msg){

    char* line = linenoise(input_msg);
    strncpy(input_buf, line, MAX_INPUT_LEN);
    free(line);

}