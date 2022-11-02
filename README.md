# FAT - Fake Awesome Terminal

<img src="./img/title.gif" alt="Title IMG"/>

Operating Systems 2021/2022 project


## Original project requirements

```
Implement a file system that uses a pseudo "FAT" on an mmapped buffer.
The functions to implement are

    createFile
    eraseFile
    write (potentially extending the file boundaries)
    read
    seek
    createDir
    eraseDir
    changeDir
    listDir
 
The opening of a file should return a "FileHandle" that stores the position in a file.
```


## Project description

"FAT - Fake Awesome Terminal" is a C implementation of some basic commands of the Linux terminal.
It serves as a CLI for a simulated underlying FAT filesystem that operates on a memory located buffer.

The [DISK](./src/fs/disk.h) and the [FAT](./src/fs/fat.h) are allocated data structures in memory that get initialized at start time.

The DISK can contain up to 3.5 MiB (3670016 bytes) of data (a reference to the [IBM 350](https://www.ibm.com/ibm/history/exhibits/storage/storage_350.html), the [first commercially used hard disk drive](https://en.wikipedia.org/wiki/History_of_hard_disk_drives)) on a char buffer.

Even if the DISK buffer is a contiguous region of memory, it gets subdivided in blocks, each 512 bytes big, thanks to the FAT filesystem access method.

The FAT itself occupies the first 70 blocks of the disk, and it is a table consisting of two values per entry:
    - char isFull: a flag that is set to 1 if the block contains valid data or 0 otherwise
    - int next: the index of the next block of the file on disk or -1 if there is no next block

Files and folders created are saved on disk respectively with [FileObject structs and FolderObject structs](./src/fs/file.h) that are the same size of a memory block, and contain bookkeeping data and the first chunk of user data. Each successive data block consists of 512 bytes of char, in the case of files, or 512 bytes of int (128 ints) in the case of folders.

Files' data field is a char buffer, while folders' data field is an array of integers, each representing the index of the first block of a contained file/folder.

The user operates on hers/his CWD (current working directory) using a FolderHandle struct in memory, that gets updated every time the CWD changes and stores basic data of the contained files and folders in two separate lists of FolderListElem struct, to "speed up" some operations (reading from disk is equivalent to read from memory in our case).

For more in depth explanations of how the single commands work, see the [commands explanation](#commands-explanation) section.


## How to use

The readline library is needed for the program to work:

```bash
sudo apt install libreadline-dev
```

Then just go in the src/ folder and run the `make` command to compile.

The program can be executed both from src/fat.bin binary or from the [./FAT](./FAT) script (recommended).

**NOTE:** the readline library causes a constant amount of memory leak, it will get changed with the [linenoise](https://github.com/antirez/linenoise) library.


## Commands

- [x] **quit**: exits FAT
- [x] **echo \<string\>**: writes the argument to stdout
- [x] **mk \<name\>**: creates the file passed as argument
- [x] **rm \<file\>**: deletes the file passed as argument
- [x] **cat \<file\>**: prints the content of the file passed as argument to stdout
- [x] **find \<name\>**: searches for a file with a matching name as the passed argument
- [x] **mkdir \<name\>**: creates a directory passed as argument
- [x] **rmdir \<dir\>**: deletes the directory passed as argument
- [x] **cd \<dir\>**: changes the current working directory
- [x] **ls**: lists the content of the current working directory
- [ ] **edit \<file\>**: allows to edit the file passed as argument with [Kilo](https://github.com/antirez/kilo) text editor
- [x] **write \<file\>**: allows to write the new content of the file from scratch (**will be changed with edit**)
- [x] **save**: saves the current FAT session to a file


## Commands explanation

- **mk**:
- **rm**:
- **cat**:
- **find**:
- **mkdir**:
- **rmdir**:
- **cd**:
- **ls**:
