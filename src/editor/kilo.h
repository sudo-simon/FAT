/*
Code based on antirez's kilo (https://github.com/antirez/kilo)
*/

#pragma once
#include "../fs/disk.h"
#include "../fs/fat.h"
#include "../fs/file.h"

int _KILO_start(char* filename, int file_len, int write_pipe_fd);