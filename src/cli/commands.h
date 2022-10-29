#pragma once

#include "../constants.h"


/*
exits FAT, takes no arguments
*/
int _quit(void* arg);

/*
clears the terminal, takes no arguments
*/
int _clear(void* arg);

/*
prints the argument string to stdout
*/
int _echo(void* arg);

/*
creates a file with the name passed as an argument
*/
int _mk(void* arg);

/*
deletes the file passed as an argument
*/
int _rm(void* arg);

/*
prints the content of the file passed as and argument to stdout
*/
int _cat(void* arg);

/*
saves the file to "disk", fs function, remove from here
*/
int writeFile(void* arg);

/*
reads the file from "disk", fs function, remove from here
*/
int readFile(void* arg);

/*
searches for the file with the name passed as an argument
*/
int _find(void* arg);

/*
creates a directory with the name passed as an argument
*/
int _mkdir(void* arg);

/*
deletes the directory passed as an argument
*/
int _rmdir(void* arg);

/*
changes the current working directory
*/
int _cd(void* arg);

/*
lists the content of the current working directory or of the directory passed as an argument
*/
int _ls(void* arg);

/*
allows to edit the file passed as an argument
*/
int _edit(void* arg);




/*
Generic function pointer used in the FN_ARRAY
*/
typedef int (*FN_PTR)(void*);

/*
Array of all the terminal commands
*/
const static char* CMD_ARRAY[TOTAL_COMMANDS] = {
    "quit",     // 0
    "clear",    // 1
    "echo",     // 2
    "mk",       // 3
    "rm",       // 4
    "cat",      // 5
    "find",     // 6
    "mkdir",    // 7
    "rmdir",    // 8
    "cd",       // 9
    "ls",       // 10
    "edit"      // 11
    //TODO: comando FAT <--new / file> per apertura di fork()?
};

/*
Ordered array of the function pointers corresponding to the commands in CMD_ARRAY
*/
const static FN_PTR FN_ARRAY[TOTAL_COMMANDS] = {
    _quit,      // 0
    _clear,     // 1
    _echo,      // 2
    _mk,        // 3
    _rm,        // 4
    _cat,       // 5
    _find,      // 6
    _mkdir,     // 7
    _rmdir,     // 8
    _cd,        // 9
    _ls,        // 10
    _edit       // 11
};