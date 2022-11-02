/*
Code based on antirez's kilo (https://github.com/antirez/kilo)
*/

#pragma once
#include <sys/ucontext.h>
#include <ucontext.h>
#include "../fs/disk.h"
#include "../fs/fat.h"
#include "../fs/file.h"

int _EDITOR_start(char* filename, ucontext_t main_context);