# FAT - Fake Awesome Terminal

<img src="./img/title.gif" alt="Title IMG"/>


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
- char isFull: a flag that is set to 1 if the block contains valid data, 0 otherwise
- int next: the index of the next block of the file on disk or -1 if there is no next block

Files and folders created are saved on disk respectively with [FileObject structs and FolderObject structs](./src/fs/file.h) that are the same size of a memory block, and contain bookkeeping data and the first chunk of user data. Each successive data block consists of 512 bytes of char, in the case of files, or 512 bytes of int (128 ints) in the case of folders.

Files' data field is a char buffer, while folders' data field is an array of integers, each representing the index of the first block of a contained file/folder.

The user operates on their CWD (current working directory) using a FolderHandle struct in memory that gets updated every time the CWD changes, and stores basic data of the contained files and folders in two separate lists of FolderListElem struct to "speed up" some operations (reading from disk is equivalent to read from memory in our case).

For more in depth explanations of how the single commands work, see the [commands details](#commands-details) section.


## How to use

Go in the src/ folder and run the `make` command to compile.

The program can then be executed both from src/fat.bin binary or from the [./FAT](./FAT) script (recommended).


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
- [x] **edit \<file\>**: allows to edit the file passed as argument with [Kilo](https://github.com/antirez/kilo) text editor
- [x] **write \<file\>**: allows to write the new content of the file from scratch (**will be changed with edit**)
- [x] **save**: saves the current FAT session to a file


## Commands details

### mk

It creates a new FileObject struct and initializes it with the correct values, and the input \<name\> as its fileName value.

The FAT gets then updated with the new file block, that is the first free one available, the file gets inserted in the directory tree as a content of the CWD and the created FileObject is saved on DISK.

```c
int _FILE_createFile(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* file_name){
    
    FileObject* new_file = calloc(1, sizeof(struct FileObject));
    new_file->isFolder = 0;
    strncpy(new_file->fileName, file_name, MAX_FILENAME_LEN);
    new_file->nextBlockIndex = -1;
    new_file->size = 0;
    memset(new_file->firstDataBlock, 0, FIRST_DATA_BLOCK_SIZE);

    int block = _FAT_findFirstFreeBlock(FAT);
    if (block == -1){
        printf("[ERROR] No space left on DISK!\n");
        return -1;
    }
    if (_FAT_allocateBlock(FAT, block) != 0) return -1;
    
    if (_DISK_writeBytes(DISK, block, (char*) new_file, sizeof(struct FileObject)) == -1) return -1;
    if (_FILE_folderAddFile(DISK, FAT, CWD, new_file, block) == -1) return -1;
    
    free(new_file);
    return block;

}
```

### rm

It searches for a matching file name in the CWD, and if successful deletes all FAT entries for said file, but does not erase file blocks (to emulate real life FAT filesystems behaviour).

The name search in the CWD takes O(log(n)) time because FolderListElem entries are kept in alphabetical order with a merge sort algorithm and then searched with a binary search one.

The file gets then removed from the directory tree, both in the CWD struct and on DISK.

```c
int _FILE_deleteFile(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* file_name){
    
    int cwd_index = _FILE_searchFileInCWD(CWD, file_name);
    if (cwd_index == -1){
        printf("No file named %s\n",file_name);
        return -1;
    }

    FileObject* file_object = (FileObject*) _DISK_readBytes(
        DISK, 
        CWD->fileList[cwd_index]->firstBlockIndex, 
        sizeof(struct FileObject)
    );

    int b_index = CWD->fileList[cwd_index]->firstBlockIndex;
    int next_index = file_object->nextBlockIndex;
    
    // Procedurally deallocate FAT entries
    while (b_index != -1){
        _FAT_deallocateBlock(FAT, b_index);
        b_index = next_index;
        next_index = _FAT_getNextBlock(FAT, next_index);
    }

    if (_FILE_folderRemoveFile(DISK, FAT, CWD, CWD->fileList[cwd_index]->firstBlockIndex) != 0){
        printf("[ERROR] Impossible to remove file from folder!\n");
        return -1;
    }

    // Putting the element to be removed at the end of the list for the realloc
    FolderListElem* tmp = CWD->fileList[cwd_index];
    CWD->fileList[cwd_index] = CWD->fileList[CWD->numFiles-1];
    CWD->fileList[CWD->numFiles-1] = tmp;

    free(CWD->fileList[CWD->numFiles-1]);
    CWD->fileList = realloc(CWD->fileList, ((CWD->numFiles-1) * sizeof(struct FolderListElem*)));
    CWD->size--;
    CWD->numFiles--;

    // Mantaining the alphabetical sorting
    _AUX_alphabeticalSort(CWD->fileList, CWD->numFiles);

    free(file_object);
    return 0;

}
```

### cat

It reads the file contents starting from the first chunk contained in the FileObject struct, and then continues to read successive blocks of bytes until the last block is read.

```c
int _FILE_getFileContent(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* file_name, char* dest_buffer){
    
    int cwd_index = _FILE_searchFileInCWD(CWD, file_name);
    if (CWD->fileList[cwd_index]->size == 0) return -1;

    int b_index = CWD->fileList[cwd_index]->firstBlockIndex;
    int next_index = _FAT_getNextBlock(FAT, b_index);

    int n_bytes = CWD->fileList[cwd_index]->size;
    int read_bytes = 0;
    
    while(b_index != -1){

        // I am reading the first file block
        if (read_bytes == 0){
            FileObject* file = (FileObject*) _DISK_readBytes(DISK, b_index, sizeof(struct FileObject));
            strncpy(dest_buffer, file->firstDataBlock, ( n_bytes < FIRST_DATA_BLOCK_SIZE ? n_bytes : FIRST_DATA_BLOCK_SIZE ));
            read_bytes = strlen(dest_buffer);
            free(file);
        }

        // I am reading the next file block(s)
        else{
            char* data_block = _DISK_readBytes(DISK, b_index, BLOCK_SIZE);
            strncpy(dest_buffer+read_bytes, data_block, ( n_bytes-read_bytes < BLOCK_SIZE ? n_bytes-read_bytes : BLOCK_SIZE ));
            read_bytes = strlen(dest_buffer);
            free(data_block);
        }

        b_index = next_index;
        next_index = _FAT_getNextBlock(FAT, b_index);
    }

    return 0;
}
```

### find

It uses a recursive function to search a file or a folder with the input \<name\> starting from the CWD, and returns all the found paths to these files and folders in a string pointer passed as a parameter.

It takes O(n*log(n)) time as a binary search is performed on every subfolder visited, and all n subfolders get visited. It is a form of depth-first search on the directory tree.

```c
void _FILE_recursiveNameSearch(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* current_dir, char* name, char** found_paths, int* found, char* current_path, int chars_to_delete){

    // Max number of results reached
    if (*found >= 100) return;

    // I look for "name" in the current_dir
    int file_cwd_index = _FILE_searchFileInCWD(current_dir, name);
    if (file_cwd_index != -1){
        char* found_name = current_dir->fileList[file_cwd_index]->name;
        found_paths[*found] = calloc(strlen(current_path)+strlen(found_name), sizeof(char));

        strcpy(found_paths[*found], current_path);
        strcat(found_paths[*found], found_name);

        (*found)++;
    }

    // Repeated check after a possible allocation of found_paths
    if (*found >= 100) return;

    int folder_cwd_index = _FILE_searchFolderInCWD(current_dir, name);
    if (folder_cwd_index != -1){
        char* found_name = current_dir->folderList[folder_cwd_index]->name;
        found_paths[*found] = calloc(strlen(current_path)+strlen(found_name), sizeof(char));

        strcpy(found_paths[*found], current_path);
        strcat(found_paths[*found], found_name);
        strcat(found_paths[*found], "/");

        (*found)++;
    }

    // Recursive path is over
    if (current_dir->numFolders == 0){
        
        //Deleting chars_to_delete from current_path (removing this branch depth)
        char s[strlen(current_path)-chars_to_delete];
        strncpy(s, current_path, strlen(current_path)-chars_to_delete);
        strcpy(current_path, s);

        return;
    }

    // Recursive steps
    for (int i=0; i<current_dir->numFolders; ++i){

        FolderObject* next_dir_obj = (FolderObject*) _DISK_readBytes(
            DISK, 
            current_dir->folderList[i]->firstBlockIndex, 
            sizeof(struct FolderObject)
        );

        // Path too long, folder ignored
        if ((strlen(current_path)+strlen(next_dir_obj->folderName)+1) > 1024){
            free(next_dir_obj);
            continue;
        }

        FolderHandle* next_dir_handle = calloc(1,sizeof(struct FolderHandle));
        strncpy(next_dir_handle->folderName, next_dir_obj->folderName, MAX_FILENAME_LEN);
        next_dir_handle->numFiles = next_dir_obj->numFiles;
        next_dir_handle->fileList = _FILE_getContainedFiles(DISK, FAT, next_dir_obj);
        next_dir_handle->numFolders = next_dir_obj->numFolders;
        next_dir_handle->folderList = _FILE_getContainedFolders(DISK, FAT, next_dir_obj);

        free(next_dir_obj);

        strcat(current_path, next_dir_handle->folderName);
        strcat(current_path, "/");

        // Actual recursive call
        _FILE_recursiveNameSearch(
            DISK, 
            FAT, 
            next_dir_handle, 
            name, 
            found_paths, 
            found, 
            current_path,
            (chars_to_delete + strlen(next_dir_handle->folderName) +1)
        );        

        free(next_dir_handle);

    }

}


int _FILE_findPathsTo(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* name, char** found_paths){

    char current_path[1024];
    strcpy(current_path, CWD->folderName);
    strcat(current_path, "/");

    int found = 0;
    
    _FILE_recursiveNameSearch(DISK, FAT, CWD, name, found_paths, &found, current_path, 0);

    return found;

}
```

### mkdir

It creates a new FolderObject struct and initializes it with the correct values, and the input \<name\> as its folderName value.

The FAT gets then updated with the new folder block, that is the first free one available, the folder gets inserted in the directory tree as a content of the CWD and the created FolderObject is saved on DISK.

```c
int _FILE_createFolder(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* folder_name){

    FolderObject* new_folder = calloc(1, sizeof(struct FolderObject));
    new_folder->isFolder = 1;
    strncpy(new_folder->folderName, folder_name, MAX_FILENAME_LEN);
    new_folder->nextBlockIndex = -1;
    new_folder->size = 0;
    new_folder->numFolders = 0;
    new_folder->numFiles = 0;
    new_folder->previousFolderBlockIndex = CWD->firstBlockIndex;
    for (int i=0; i<CONTENT_LIST_BLOCKS_SIZE; ++i){
        new_folder->contentListBlocks[i] = -1;
    }

    int block = _FAT_findFirstFreeBlock(FAT);
    if (block == -1){
        printf("[ERROR] No space left on DISK!\n");
        return -1;
    }
    if (_FAT_allocateBlock(FAT, block) != 0) return -1;
    
    if (_DISK_writeBytes(DISK, block, (char*) new_folder, sizeof(struct FolderObject)) == -1) return -1;
    if (_FILE_folderAddFolder(DISK, FAT, CWD, new_folder, block) == -1) return -1;
    
    free(new_folder);
    return block;

}
```

### rmdir

It searches for a matching folder name in the CWD, and if successful deletes all FAT entries for said folder, but does not erase file blocks (to emulate real life FAT filesystems behaviour).

If the folder is not empty, the user is prompted with a request to delete all contained files and folders recursively.

The name search in the CWD takes O(log(n)) time because FolderListElem entries are kept in alphabetical order with a merge sort algorithm and then searched with a binary search one.

The folder gets then removed from the directory tree, both in the CWD struct and on DISK.

```c
int _FILE_deleteFolder(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* folder_name, char recursive_flag){

    int cwd_index = _FILE_searchFolderInCWD(CWD, folder_name);
    if (cwd_index == -1){
        printf("No folder named %s\n",folder_name);
        return -1;
    }

    FolderObject* folder_object = (FolderObject*) _DISK_readBytes(
        DISK, 
        CWD->folderList[cwd_index]->firstBlockIndex, 
        sizeof(struct FolderObject)
    );

    int b_index = CWD->folderList[cwd_index]->firstBlockIndex;
    int next_index = folder_object->nextBlockIndex;
    
    // Recursively deallocates subfolders and files if present
    if (folder_object->size > 0){

        if (! recursive_flag){
            char ans;
            printf("The folder contains other files and folders.\nDo you want to recursively remove them all? [y/n]: ");
            scanf("%c",&ans);
            if (ans == 'n' || ans == 'N'){
                free(folder_object);
                return 0;
            }
            else if (ans == 'y' || ans == 'Y'){
                _FILE_recursiveFolderDelete(DISK, FAT, CWD, folder_object);
            }
            else{
                printf("%c is not a valid answer\n",ans);
                free(folder_object);
                return 0;
            }
        }

        else _FILE_recursiveFolderDelete(DISK, FAT, CWD, folder_object);

    }

    // Procedurally deallocate FAT entries
    while (b_index != -1){
        _FAT_deallocateBlock(FAT, b_index);
        b_index = next_index;
        next_index = _FAT_getNextBlock(FAT, next_index);
    }

    if (_FILE_folderRemoveFolder(DISK, FAT, CWD, CWD->folderList[cwd_index]->firstBlockIndex) != 0){
        printf("[ERROR] Impossible to remove folder %s from folder!\n",folder_name);
        return -1;
    }

    // Putting the element to be removed at the end of the list for the realloc
    FolderListElem* tmp = CWD->folderList[cwd_index];
    CWD->folderList[cwd_index] = CWD->folderList[CWD->numFolders-1];
    CWD->folderList[CWD->numFolders-1] = tmp;

    free(CWD->folderList[CWD->numFolders-1]);
    CWD->folderList = realloc(CWD->folderList, ((CWD->numFolders-1) * sizeof(struct FolderListElem*)));
    CWD->size--;
    CWD->numFolders--;

    // Mantaining the alphabetical sorting
    _AUX_alphabeticalSort(CWD->folderList, CWD->numFolders);

    free(folder_object);
    return 0;

}
```

### cd

It changes all the fields of the CWD FolderHandle struct in memory to match the contents of the folder passed as input.

```c
int _FILE_changeWorkingDirectory(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* new_WD_name){

    int b_index;

    if (strncmp(new_WD_name, "..", MAX_FILENAME_LEN) == 0){
        if (CWD->previousFolderBlockIndex == -1){
            printf("%s folder has no parent folder\n",CWD->folderName);
            return -1;
        }
        b_index = CWD->previousFolderBlockIndex;
    }
    else b_index = CWD->folderList[_FILE_searchFolderInCWD(CWD, new_WD_name)]->firstBlockIndex;
    

    int next_index = _FAT_getNextBlock(FAT, b_index);

    int n_folders = CWD->numFolders;
    int n_files = CWD->numFiles;

    FolderObject* new_CWD = (FolderObject*) _DISK_readBytes(
        DISK, 
        b_index, 
        sizeof(struct FolderObject)
    );

    // Swapping CWD fields
    strncpy(CWD->folderName, new_CWD->folderName, MAX_FILENAME_LEN);
    CWD->firstBlockIndex = b_index;

    while (b_index != -1){
        CWD->currentBlockIndex = b_index;
        b_index = next_index;
        next_index = _FAT_getNextBlock(FAT, b_index);
    }

    CWD->size = new_CWD->size;
    CWD->previousFolderBlockIndex = new_CWD->previousFolderBlockIndex;

    // Freeing all previously allocated struct pointers
    for (int i=0; i<n_folders; ++i){
        free(CWD->folderList[i]);
    }
    for (int i=0; i<n_files; ++i){
        free(CWD->fileList[i]);
    }
    free(CWD->folderList);
    free(CWD->fileList);

    CWD->numFolders = new_CWD->numFolders;
    CWD->folderList = _FILE_getContainedFolders(DISK, FAT, new_CWD);
    CWD->numFiles = new_CWD->numFiles;
    CWD->fileList = _FILE_getContainedFiles(DISK, FAT, new_CWD);

    free(new_CWD);
    return 0;

}
```

### ls

It lists all contents of CWD to stdout, together with the total number of contents present and the size of each one, bytes in the case of files and contents in the case of folders.

The DISK is never used for this operation as the CWD struct kept in memory keeps track of its contents and their sizes.

```c
int _FS_listDir(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD){
    
    printf("Total %d\n",CWD->size);

    for (int i=0; i<CWD->numFolders; ++i){
        FolderListElem* folder = CWD->folderList[i];
        printf("[%d elems] %s/\n",folder->size, folder->name);
    }

    for (int i=0; i<CWD->numFiles; ++i){
        FolderListElem* file = CWD->fileList[i];
        printf("[%d bytes] %s\n",file->size, file->name);
    }

    return 0;

}
```

### edit

It opens the file passed as argument in the Kilo text editor: does so by forking into a subprocess that reads the needed file from it's copy of DISK, and then saves every subsequent edit on his temporary buffer.

When the user quits Kilo, the new file size and its new content get written on a pipe that was originally passed as an argument to the _KILO_start() function.

```c
int _edit(void *arg){
    
    char* file_name = (char*) arg;
    if (strlen(file_name) == 0){
        printf("Name of the file to edit is needed\n");
        return -1;
    }
    else if (strlen(file_name) > MAX_FILENAME_LEN){
        printf("File name too long! It can be a maximum of %d characters long\n",MAX_FILENAME_LEN);
        return -1;
    }

    if (! _FILE_existingFileName(CWD, file_name)){
        printf("There is no file named %s in this folder\n",file_name);
        return -1;
    }

    // ----------------------- EDITOR START -----------------------------

    EDITOR_OPEN = 1;

    char* file_content;
    int file_len = CWD->fileList[_FILE_searchFileInCWD(CWD, file_name)]->size;

    if (file_len == 0){
        file_content = calloc(2,1);
        strcpy(file_content, "");
    }
    else{
        file_content = calloc(file_len, sizeof(char));
        if (_FILE_getFileContent(DISK, FAT, CWD, file_name, file_content) == -1){
            printf("[ERROR]: unable to read %s file",file_name);
            goto editor_end;
        }
    }
    

    // ----------------- PIPE CREATION -----------------
    int editor_pipe[2]; // [0] = r --- [1] = w
    if (pipe(editor_pipe) == -1){
        printf("[ERROR]: IPC handling\n");
        goto editor_end;
    }
    
    
    // ---------------------- FORK ---------------------
    pid_t editor_pid = fork();
    if (editor_pid == -1){
        printf("[ERROR]: fork error!\n");
        close(editor_pipe[1]);
        goto editor_end;
    }


    // KILO EDITOR PROCESS
    if (editor_pid == 0){
        close(editor_pipe[0]); // Closing read pipe
        _KILO_start(file_name, file_len, editor_pipe[1]);
        free(file_content);
        //exit(0);
        return 0;
    }

    // FAT MAIN PROCESS
    else{
        close(editor_pipe[1]);  // Closing write pipe

        int kilo_exit_status;
        wait(&kilo_exit_status);
        
        if (kilo_exit_status == -1){
            goto editor_end;
        }

        // First 4 bytes of the pipe are the new file length
        int new_len;
        if (read(editor_pipe[0], &new_len, sizeof(int)) == -1){
            printf("[ERROR] Unable to read form pipe!\n");
            goto editor_end;
        }

        // Rest of the pipe is the new file content
        char* saved_file = calloc(new_len, 1);
        if (read(editor_pipe[0], saved_file, new_len) == -1){
            printf("[ERROR] Unable to read form pipe!\n");
            free(saved_file);
            goto editor_end;
        }

        if (_FILE_writeFileContent(DISK, FAT, CWD, file_name, saved_file, new_len) == -1){
            printf("[ERROR] Unable to write on DISK!\n");
        }
        free(saved_file);

    }

    // ----------------------- EDITOR STOP -----------------------------


editor_end:
    close(editor_pipe[0]);
    shell_init();
    EDITOR_OPEN = 0;
    free(file_content);
    return 0;

}
```
