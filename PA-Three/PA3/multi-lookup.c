#include "multi-lookup.h"


void* requesterThreads(void * inputFiles){
    // unpack argument
    requestThreadArg* arg = (requestThreadArg*) inputFiles;
    
    pthread_exit(NULL);
 }


 void* resolverThreads(void * inputFiles){
    // unpack argument
    resolverThreadArg* arg = (resolverThreadArg*) inputFiles;

    pthread_exit(NULL);
 }

/* checks for valid file name input */
int isValidFile(char *filename){
    
    // concatonate the directory path
    char* extension = "input/";
    char* newPath = malloc(strlen(filename)+1+6); 
    strcpy(newPath, extension); /* copy name into the new var */
    strcat(newPath, filename); /* add the extension */

    FILE *fptr = fopen(newPath, "r");
    if (fptr == NULL){
        return -1;
    }
    fclose(fptr);
    return 1;
}

// need to create the .h file because the default make file is looking for it
// <# requester> <# resolver> <requester log> <resolver log> [ <data file> ... ]
int main(int argc, char* argv[] ) {
    /* timer */
    struct timeval begin, end;
    gettimeofday(&begin, NULL);

    /* Array of threads*/


    /* parsing the input */
    int requesterThreadsCount = atoi(argv[1]);
    int resolverThreadCount = atoi(argv[2]);
    char* requesterlog = argv[3];
    char* resolverlog = argv[4];

    if (argc < 6 || requesterThreadsCount <1 || resolverThreadCount < 1) {
        printf("Not enough command line arguments. Stopping program.\n");
        return -1;
    }
    if (requesterThreadsCount > MAX_REQUESTER_THREADS || resolverThreadCount > MAX_RESOLVER_THREADS) {
        printf("You are requesting to many threads. Stopping program\n");
        return -1;
    }

    pthread_t reqWorkers [requesterThreadsCount];
    pthread_t resWorkers [resolverThreadCount];

    /* Initializing the inputFiles struct */
    inputFiles* input_files;  
    input_files = malloc(sizeof(*input_files) + sizeof(file) * argc-5);
    input_files->currentFileIndex = 0;
    int fileCount = 0;
    for (int i = 5; i < argc; i++) {
        file file;
        file.filename = argv[i];
        file.serviced = false;
        int isValid = isValidFile(file.filename);
        if (isValid == 1) {  
            fileCount = fileCount + 1;
            input_files->files[i] = file;
            //printf("Valid file: %s\n", input_files->files[i].filename);
        }
    }
    input_files->totalFileCount = fileCount; 
    pthread_mutex_init(&input_files->file_lock, NULL);
    
    /* Initializing the sharedBuffer */
    sharedBuffer* shared;
    shared = malloc(sizeof(*shared) + sizeof(char*) * ARRAY_SIZE);
    shared->currentPosition = 0;
    pthread_mutex_init(&shared->buffer_lock, NULL);
    pthread_cond_init(&shared->isEmpty, NULL);
    pthread_cond_init(&shared->isFull, NULL);

    /* Initializing the requester threads argument */
    requestThreadArg requester;
    requester.servicelog = requesterlog;
    requester.files = input_files;
    requester.buffer = shared;
    pthread_mutex_init(&requester.service_lock, NULL);
    
    /* Initializing the resolver threads argument */    
    resolverThreadArg resolver;
    resolver.resultslog = resolverlog;
    resolver.buffer = shared;
    pthread_mutex_init(&resolver.results_lock, NULL);

    printf("Requester thread count %d\n", requesterThreadsCount);
    printf("Resolver thread count %d\n", resolverThreadCount);
    printf("Requester log name: %s\n", requesterlog);
    printf("Resolver log name: %s\n", resolverlog);



    /* start the threads */
    for (int i = 0; i < requesterThreadsCount; i++) {
        pthread_create(&reqWorkers[i],NULL,requesterThreads, &requester);
    }
    for (int k = 0; k < resolverThreadCount; k++) {
        pthread_create(&resWorkers[k],NULL,resolverThreads, &resolver);
    }

    /* Wait for the threads to finish, join them */
    for (int j = 0; j < requesterThreadsCount; j++) {
        pthread_join(reqWorkers[j],NULL);
    }
    for (int f = 0; f < resolverThreadCount; f++) {
        pthread_join(resWorkers[f],NULL);
    }

    /* Cleanup all the memory you created */
    pthread_mutex_destroy(&shared->buffer_lock);
    pthread_mutex_destroy(&input_files->file_lock);
    pthread_mutex_destroy(&resolver.results_lock);
    pthread_mutex_destroy(&requester.service_lock);
    pthread_cond_destroy(&shared->isEmpty);
    pthread_cond_destroy(&shared->isFull);
    free(input_files);
    free(shared);

    /* stop the time tracker */
    gettimeofday(&end, NULL);
    double seconds = (end.tv_sec - begin.tv_sec) + ((end.tv_usec - begin.tv_usec)/1000000.0);  /* convert it into seconds */
    printf("./multi-lookup: total time is %f seconds\n", seconds);

    /* exit the program */
    exit(0);
}