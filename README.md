# FAT - Fake Awesome Terminal

<img src="./img/title.gif" alt="Title IMG" width=500/>

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

## Commands

- **quit**: exits FAT
- **echo \<string\>**: writes the argument to stdout
- **mk \<name\>**: creates the file passed as argument
- **rm \<file\>**: deletes the file passed as argument
- **cat \<file\>**: prints the content of the file passed as argument on the terminal
- **find \<name\>**: searches for a file with a matching name as the passed argument
- **mkdir \<name\>**: creates a directory passed as argument
- **rmdir \<dir\>**: deletes the directory passed as argument
- **cd \<dir\>**: changes the current working directory
- **ls \<dir?\>**: lists the content of the current directory or the one passed as argument (if present) 
- **edit \<file\>**: allows to edit the file passed as argument