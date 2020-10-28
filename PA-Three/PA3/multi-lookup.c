#include "multi-lookup.h"

 /** attaches extension for filepath */
 char* getExtension(char *filename) {
    char* extension = "input/";
    char* newPath = malloc(strlen(filename)+1+6); 
    strcpy(newPath, extension); /* copy name into the new var */
    strcat(newPath, filename); /* add the extension */
    return newPath; 
 }

/** checks for valid file name input */
int isValidFile(char *filename){
    char* newPath = getExtension(filename);
    FILE *fptr = fopen(newPath, "r");
    if (fptr == NULL){
        return -1;
    }
    free(newPath);
    fclose(fptr);
    return 1;
}

/** Clears serviced.txt and results.txt */
void clearLogs(){
    FILE* serviceFile = fopen("serviced.txt", "w");  
    FILE* resultsFile = fopen("results.txt", "w");

    fprintf(serviceFile, "");
    fprintf(resultsFile, "");

    fclose(serviceFile);
    fclose(resultsFile);
}


/* Upto this point you need to figure out how start adding items to the buffer and start using the condition variables */
void* requesterThreads(void * inputFiles){
    requestThreadArg* arg = (requestThreadArg*) inputFiles;
    char* servicetxtfile = arg->servicelog;
    FILE* serviceFile = fopen(servicetxtfile, "a");  // Strictly reading mode 

    bool canService = true;
    FILE* currentFile;
    char buff[MAX_NAME_LENGTH];
    int serviceCount = 0;

    while(canService) {
        int i = 0;
        canService = false;

        pthread_mutex_lock(&arg->buffer->buffer_lock);  /** Grabbing an unprocessed file  */
            for (i = 0; i < arg->files->totalFileCount; i++) {
                if (!arg->files->files[i].serviced) {   // found and unserviced file, so we grab it 
                    //printf("Grabbing unserviced file: %s\n", arg->files->files[i].filename); 
                    canService = true;
                    arg->files->files[i].serviced = true;
                    currentFile = fopen(getExtension(arg->files->files[i].filename), "r+");
                    serviceCount++;
                    break;  // go and work on this file 
                }
            }
        pthread_mutex_unlock(&arg->buffer->buffer_lock);

        if (!canService) { break; } /** if thread can't find anything to service then exit */
        
        while(fscanf(currentFile, "%s", buff) == 1) {
            pthread_mutex_lock(&arg->service_lock);     /** we put the lock in here so other threads don't starve */
                fprintf(serviceFile, "%s\n", buff);
            pthread_mutex_unlock(&arg->service_lock);
        }
        fclose(currentFile);
    }
    printf("Thread %d serviced %d files.\n", (int)pthread_self(), serviceCount);
    fclose(serviceFile);
    pthread_exit(NULL);
 }

/*
typedef struct {
    pthread_mutex_t results_lock;  
    char  *resultslog;             
    sharedBuffer *buffer;         
} resolverThreadArg;
*/
 void* resolverThreads(void * inputFiles){

    resolverThreadArg* arg = (resolverThreadArg*) inputFiles;
    char* resultstxtfile = arg->resultslog;
    
    FILE * results = fopen(resultstxtfile, "a"); 

    /* FOR RIGHT NOW ONLY, just create dummy test strings so you can figure out how to the dns function */
    char* testString1 = "facebook.com";
    char* testString2 = "youtube.com";
    char* testString3 = "yahoo.com";

    //char ipString[INET_ADDRSTRLEN];
    char* ipString = malloc(sizeof(char) * INET_ADDRSTRLEN);

    if (dnslookup(testString1, ipString, INET_ADDRSTRLEN) == UTIL_SUCCESS ) {
        printf(">SUCCEEDED\n");
    }

    fprintf(results, "IPSTRING: %s\n", ipString);  // should print out the generated ip string

    fclose(results);
    pthread_exit(NULL);
 }


// need to create the .h file because the default make file is looking for it
// <# requester> <# resolver> <requester log> <resolver log> [ <data file> ... ]
int main(int argc, char* argv[]) {
    clearLogs();

    /* timer */
    struct timeval begin, end;
    gettimeofday(&begin, NULL);

    /** parsing the input */
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

    /** Array of threads*/
    pthread_t reqWorkers [requesterThreadsCount];
    pthread_t resWorkers [resolverThreadCount];

    /** Initializing the inputFiles struct */
    inputFiles* input_files;  
    input_files = malloc(sizeof(*input_files) + sizeof(file**) * argc-5);
    input_files->currentFileIndex = 0;
    int fileCount = 0;
    for (int i = 5; i < argc; i++) {
        file file;
        file.filename = argv[i];
        file.serviced = false;
        int isValid = isValidFile(file.filename);
        if (isValid == 1) {  
            fileCount = fileCount + 1;
            input_files->files[input_files->currentFileIndex] = file;
            input_files->currentFileIndex++;
        }
    }
    input_files->totalFileCount = fileCount; 

    pthread_mutex_init(&input_files->file_lock, NULL);
    
    /** Initializing the sharedBuffer */
    sharedBuffer* shared;
    shared = malloc(sizeof(*shared) + sizeof(char*) * ARRAY_SIZE);
    shared->currentPosition = 0;
    pthread_mutex_init(&shared->buffer_lock, NULL);
    pthread_cond_init(&shared->isEmpty, NULL);
    pthread_cond_init(&shared->isFull, NULL);

    /** Initializing the requester threads argument */
    requestThreadArg requester;
    requester.servicelog = requesterlog;
    requester.files = input_files;
    requester.buffer = shared;
    pthread_mutex_init(&requester.service_lock, NULL);
    
    /** Initializing the resolver threads argument */    
    resolverThreadArg resolver;
    resolver.resultslog = resolverlog;
    resolver.buffer = shared;
    pthread_mutex_init(&resolver.results_lock, NULL);

    printf("Requester thread count %d\n", requesterThreadsCount);
    printf("Resolver thread count %d\n", resolverThreadCount);

    /** start the threads */
    for (int i = 0; i < requesterThreadsCount; i++) {
        pthread_create(&reqWorkers[i],NULL,requesterThreads, &requester);
    }
    for (int k = 0; k < resolverThreadCount; k++) {
        pthread_create(&resWorkers[k],NULL,resolverThreads, &resolver);
    }

    /** Wait for the threads to finish */
    for (int j = 0; j < requesterThreadsCount; j++) {
        pthread_join(reqWorkers[j],NULL);
    }
    for (int f = 0; f < resolverThreadCount; f++) {
        pthread_join(resWorkers[f],NULL);
    }

    /** Cleanup all the memory you created */
    pthread_mutex_destroy(&shared->buffer_lock);
    pthread_mutex_destroy(&input_files->file_lock);
    pthread_mutex_destroy(&resolver.results_lock);
    pthread_mutex_destroy(&requester.service_lock);
    pthread_cond_destroy(&shared->isEmpty);
    pthread_cond_destroy(&shared->isFull);
    free(input_files);
    free(shared);

    /** stop the time tracker */
    gettimeofday(&end, NULL);
    double seconds = (end.tv_sec - begin.tv_sec) + ((end.tv_usec - begin.tv_usec)/1000000.0);  /* convert it into seconds */
    printf("./multi-lookup: total time is %f seconds\n", seconds);

    /** exit the program */
    exit(0);
}