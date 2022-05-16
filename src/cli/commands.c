
#include <stdio.h>
#include <stdlib.h>
// My headers
#include "../constants.h"
#include "../cli/shell.h"


/*

*/
int quit(void* arg){
    printf("Exiting FAT...\n");
    exit(0);
    return 0;
}

/*

*/
int echo(void* string){
    return printf("%s",((char*)string));
}

/*

*/
int createFile(void *arg){
    printf("createFile not yet implemented");
    return 0;
}

/*

*/
int eraseFile(void *arg){
    printf("eraseFile not yet implemented");
    return 0;
}

/*

*/
int writeFile(void *arg){
    printf("writeFile not yet implemented");
    return 0;
}

/*

*/
int readFile(void *arg){
    printf("readFile not yet implemented");
    return 0;
}

/*

*/
int seek(void *arg){
    printf("seek not yet implemented");
    return 0;
}

/*

*/
int createDir(void *arg){
    printf("createDir not yet implemented");
    return 0;
}

/*

*/
int eraseDir(void *arg){
    printf("eraseDir not yet implemented");
    return 0;
}

/*

*/
int changeDir(void *arg){
    printf("changeDir not yet implemented");
    return 0;
}

/*

*/
int listDir(void *arg){
    printf("listDir not yet implemented");
    return 0;
}

/*

*/
int editFile(void *arg){
    printf("editFile not yet implemented");
    return 0;
}
