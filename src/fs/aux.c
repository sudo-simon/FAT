#include <stdlib.h>
#include <string.h>
// My headers
#include "aux.h"


/* ----------------------------------------------- Sorting algorithms ------------------------------------------------- */


void blockMerge(int* arr, int left, int mid, int right){
    int i, j, k;
    int n1 = mid - left + 1;
    int n2 = right - mid;

    /* create temp arrays */
    int L[n1];
    int R[n2];

    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = left; // Initial index of merged subarray
    while (i < n1 && j < n2) {
        if (L[i] > R[j]) {     // Swapped the condition to make it sort in descending order (-1 index avoidance)
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    /* Copy the remaining elements of L[], if there
    are any */
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    /* Copy the remaining elements of R[], if there
    are any */
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}
 

void blockMergeSort(int arr[], int left, int right){
    if (left < right) {
        // Same as (l+r)/2, but avoids overflow for
        // large l and h
        int mid = left + ((right - left) / 2);
 
        // Sort first and second halves
        blockMergeSort(arr, left, mid);
        blockMergeSort(arr, mid+1, right);
 
        blockMerge(arr, left, mid, right);
    }
}



void FolderListElemMerge(FolderListElem** arr, int left, int mid, int right){
    int i, j, k;
    int n1 = mid - left + 1;
    int n2 = right - mid;

    /* create temp arrays */
    FolderListElem* L[n1];
    FolderListElem* R[n2];

    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = left; // Initial index of merged subarray
    while (i < n1 && j < n2) {
        if (strncmp(L[i]->name, R[j]->name, MAX_FILENAME_LEN) <= 0) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    /* Copy the remaining elements of L[], if there
    are any */
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    /* Copy the remaining elements of R[], if there
    are any */
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}
 

void FolderListElemMergeSort(FolderListElem** arr, int left, int right){
    if (left < right) {
        // Same as (l+r)/2, but avoids overflow for
        // large l and h
        int mid = left + ((right - left) / 2);
 
        // Sort first and second halves
        FolderListElemMergeSort(arr, left, mid);
        FolderListElemMergeSort(arr, mid+1, right);
 
        FolderListElemMerge(arr, left, mid, right);
    }
}


/* ------------------------------------------------------------------------------------------------------------- */


// Merge sort algorithm
void _AUX_alphabeticalSort(FolderListElem** elem_list, int list_len){
    FolderListElemMergeSort(elem_list, 0, list_len-1);
}

// Merge sort algorithm
void _AUX_blockSort(int* block_list, int list_len){
    blockMergeSort(block_list, 0, list_len-1);
}

int _AUX_intBinarySearch(int* list, int list_len, int to_find){
    int left = 0;
    int right = list_len - 1;
    while (left <= right){
        int mid = left + ((right-left) / 2);
        int res = to_find < list[mid] ? -1 : to_find > list[mid] ? +1 : 0;
        if (res == 0) return mid;
        else if (res > 0) left = mid+1;
        else right = mid-1;
    }
    return -1;
}

//int _AUX_fileBinarySearch(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* file_name);

//int _AUX_folderBinarySearch(DISK_STRUCT* DISK, FAT_STRUCT* FAT, FolderHandle* CWD, char* folder_name);

int _AUX_validateInput(char* input_str){
    // Forbidden characters for file and folder names
    char no[] = "/,\\";
    for (int i=0; i<strlen(input_str); ++i){
        for (int j=0; j<strlen(no); ++j){
            if (input_str[i] == no[j]) return 0;
        }
    }
    return 1;
}