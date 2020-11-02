#ifndef MULTI_LOOKUP_H
#define MULTI_LOOKUP_H

#include "util.h"
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/time.h>

#define ARRAY_SIZE 20
#define MAX_INPUT_FILES 10
#define MAX_RESOLVER_THREADS 10
#define MAX_REQUESTER_THREADS 5
#define MAX_NAME_LENGTH 1025

int isValidFile(char *filename);
int clearLogs();
void *requesterThreads(void * inputFiles);
void *resolverThreads(void * inputFiles);
int isValidFile(char* filename);

/* Defining structures so for the thread arguments */

typedef struct {
    int serviced;   /* allows requestor threads to determine if the current file has already been processed */
    char* filename;   /* name of the file, ex. names2.txt */
} file;

typedef struct inputFiles {         
    int currentFileIndex;            /* The most recent file thats been grabbed by a thread */
    int totalFileCount;              /* threads will loop through all til all files */
    pthread_mutex_t file_lock;  /* Requesters use this to take turns grabbing from the array of file names, grab one by one */
    file files[];
} inputFiles;


/* shared buffer between requester and resolver */
typedef struct sharedBuffer {
    int threadCount;                /* The total amount of threads that started, need this to tell resolvers to exit */
    int threadsFinished;            /* The amount of requestor threads that have finished */
    pthread_cond_t isFull;          /* Should make requesters block */
    pthread_cond_t isEmpty;         /* Should make resolvers block */
    pthread_mutex_t buffer_lock;    /* mutex to get into the shared buffer */
    int currentPosition;            /* each requester thread will update this as they need to update  */
    char* buffer[];
} sharedBuffer;


/* Requester Thread Arguments */
typedef struct requestThreadArg {
    pthread_mutex_t service_lock;   /* Lock that the requester threads need to be able to write to serviced.txt safely */
    char* servicelog;               /* The service log that thread will append too */
    inputFiles* files;              /* pointer to the inputFiles struct */
    sharedBuffer* buffer;           /* pointer to the sharedBuffer struct */
} requestThreadArg;



/* Resolver Thread Arguments */
typedef struct resolverThreadArg {
    pthread_mutex_t results_lock;  /* Lock that resolver threads need to be able to write to resolver.txt safely */
    char  *resultslog;              /* filename for resolver.txt */
    sharedBuffer *buffer;          /* pointer to the sharedBuffer struct */ 
} resolverThreadArg;
#endif