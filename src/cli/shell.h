#pragma once

/*
Clears the terminal screen at shell initialization
*/
#define clear_all() printf("\033[H\033[J")

int shell_init();

char* current_time_str();

int take_input(char* input_buf);

int str_split(char* in, char** out);
