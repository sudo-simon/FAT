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
It serves as a CLI for a simulated underlying FAT filesystem that operates on a memory mapped buffer.


## How to use

The readline library is needed for the program to work:

```bash
sudo apt install libreadline-dev
```

Then just go in the src/ folder and run the `make` command to compile.

The program can be executed both from src/fat.bin binary or from the [./FAT](./FAT) script (recommended).


## Commands

- [x] **quit**: exits FAT
- [x] **echo \<string\>**: writes the argument to stdout
- [x] **mk \<name\>**: creates the file passed as argument
- [x] **rm \<file\>**: deletes the file passed as argument
- [x] **cat \<file\>**: prints the content of the file passed as argument to stdout
- [ ] **find \<name\>**: searches for a file with a matching name as the passed argument
- [x] **mkdir \<name\>**: creates a directory passed as argument
- [x] **rmdir \<dir\>**: deletes the directory passed as argument
- [x] **cd \<dir\>**: changes the current working directory
- [x] **ls**: lists the content of the current working directory
- [x] **write \<file\>**: allows to write the new content of the file from scratch (will be swapped with **edit**)
- [ ] **edit \<file\>**: allows to edit the file passed as argument
- [x] **save**: saves the current FAT session to a file

