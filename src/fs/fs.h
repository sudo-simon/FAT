#pragma once

//TODO: i parametri delle funzioni andranno probabilmente cambiati in strutture dati inizializzate
//TODO: dai wrapper in commands.c

int _fs_createFile(char* file_name);

int _fs_eraseFile(char* file_name);

int _fs_write();

int _fs_read();

int _fs_seek(char* file_name);  //TODO: chiedere a cosa si riferisca nel dettaglio

int _fs_createDir(char* dir_name);

int _fs_eraseDir(char* dir_name);

int _fs_changeDir(char* dir_name);

int _fs_listDir(char* dir_name);