#pragma once

#define MAX_INPUT_LEN 128       // Length of input string buffer
#define MAX_HISTORY_LEN 128     // Length of command history buffer
#define TOTAL_COMMANDS 12       // Total number of commands

//#define DISK_SIZE 32768         // 32 kB DISK
//#define DISK_SIZE 524288        // 512 kB DISK
//#define DISK_SIZE 1048576       // 1 MiB DISK
#define DISK_SIZE 3670016       // 3.5 MiB DISK (IBM 350)
#define BLOCK_SIZE 256          // 256 byte block size