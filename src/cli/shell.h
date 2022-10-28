#pragma once

/*
Clears the terminal screen at shell initialization
*/
#define clear_all() printf("\033[H\033[J")

/*
Shell initialization and welcome message
*/
int shell_init();

/*
Gets the current date and time and formats it to a string
*/
char* current_time_str();

/*
Takes the user input and also implements the command history
(uses the readline-dev library)
*/
int take_input(char* input_buf, char* input_msg);

/*
Splits the input string in a maximum of 2 space separated substrings
Buffer management and segfault prevention is done by the function caller
(multiple spaces can separate arguments, so that a number of spaces isn't considered an argument)
*/
int str_split(char* in, char** out);
