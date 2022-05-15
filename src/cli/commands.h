#pragma once

#include "../constants.h"

//TODO: rinominare queste funzioni come i comandi stessi e scrivere le vere funzioni come fs_createFile...
// quit
int quit(void* arg);

// echo
int echo(void* arg);

// mk (touch?)
int createFile(void* arg);

// rm
int eraseFile(void* arg);

// save
int writeFile(void* arg);

// cat
int readFile(void* arg);

// find //TODO: chiedere a cosa si riferisca
int seek(void* arg);

// mkdir
int createDir(void* arg);

// rmdir
int eraseDir(void* arg);

// cd
int changeDir(void* arg);

// ls
int listDir(void* arg);

// edit
int editFile(void* arg);




// Generic function pointer used in the FN_ARRAY
typedef int (*FN_PTR)(void*);

// Array of all the terminal commands
const static char* CMD_ARRAY[TOTAL_COMMANDS] = {
    "quit",     // 0
    "echo",     // 1
    "mk",       // 2
    "rm",       // 3
    "cat",      // 4
    "find",     // 5
    "mkdir",    // 6
    "rmdir",    // 7
    "cd",       // 8
    "ls",       // 9
    "edit"      // 10
};

// Ordered array of the function pointers corresponding to the commands in CMD_ARRAY
const static FN_PTR FN_ARRAY[TOTAL_COMMANDS] = {
    quit,       // 0
    echo,       // 1
    createFile, // 2
    eraseFile,  // 3
    readFile,   // 4
    seek,       // 5
    createDir,  // 6
    eraseDir,   // 7
    changeDir,  // 8
    listDir,    // 9
    editFile    // 10
};