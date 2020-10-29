#include <util.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/time.h>

// PURPOSE: Just trying to file the array found inside of the 'inputFiles' struct with objects w/ type 'file'
typedef struct {
    bool serviced;   /* allows requestor threads to determine if the current file has already been processed */
    char* filename;   /* name of the file, ex. names2.txt */
} file;


typedef struct {         
    int currentFileIndex;            /* The most recent file thats been grabbed by a thread */
    int totalFileCount;              /* threads will loop through all til all files */
    pthread_mutex_t file_lock;  /* Requesters use this to take turns grabbing from the array of file names, grab one by one */
    file files[];
} inputFiles;


int main(int argc, char* argv[]) {
    
    /* Initialize inputFiles struct */
    inputFiles* input_files;  
    input_files = malloc(sizeof(*input_files) + sizeof(file*) * 3);  // enough for five files 
    input_files->currentFileIndex = 0;

    /* creating 3 files that will be stored inside of inputFiles */
    file one, two, three;
    one.filename = "names1.txt";
    one.serviced = false;
    two.filename = "names2.txt";
    two.serviced = false;
    three.filename = "names3.txt";
    three.serviced = false; 

    /* now trying to load the file */
    input_files->files[input_files->currentFileIndex] = one;
    input_files->currentFileIndex++;
    input_files->files[input_files->currentFileIndex] = two;
    input_files->currentFileIndex++;
    input_files->files[input_files->currentFileIndex] = three;
    input_files->currentFileIndex++;
    printf("first index value: %s\n", input_files->files[input_files->currentFileIndex-1].filename); // expect names1.txt
    printf("first index value: %s\n", input_files->files[input_files->currentFileIndex-2].filename); // expect names2.txt
    printf("first index value: %s\n", input_files->files[input_files->currentFileIndex-3].filename); // expect names3.txt


    


    return 0;
}