#include "multi-lookup.h"


/** checks for valid file name input */
int isValidFile(char *filename){
    FILE *fptr = fopen(filename, "r");
    if (fptr == NULL || strlen(filename) <= 6){    // the 'i' is if they did something like /run ... /input  input/names1.txt
        fprintf(stderr, "Bogus file name %s \n", filename);
        return -1;
    }
    fclose(fptr);
    return 1;
}

/** Clears serviced.txt and results.txt */
int clearLogs(char* service, char* results){
    FILE* serviceFile = fopen(service, "w");  
    FILE* resultsFile = fopen(results, "w");

    /* failed to open output paths */
    if (serviceFile == NULL) {
        printf(stderr, "Bogus output file path: %s\n", serviceFile);
        return -1;
    }

    if (results == NULL) {
        printf(stderr, "Bogus output file path: %s\n", results);
        return -1;
    }

    fprintf(serviceFile, "%s", "");
    fprintf(resultsFile, "%s", "");

    fclose(serviceFile);
    fclose(resultsFile);
    return 1;
}

void* requesterThreads(void * inputFiles){
    requestThreadArg* arg = (requestThreadArg*) inputFiles;
    char* servicetxtfile = arg->servicelog;

    int canService = 1;
    FILE* currentFile;
    int serviceCount = 0;

    while(canService == 1) {
        int i = 0;
        canService = 0;

        /** Thread will go into the list of files and look for one that is unserviced */
        pthread_mutex_lock(&arg->files->file_lock);  /** Only single requestor thread may search through list of files  */
            for (i = 0; i < arg->files->totalFileCount; i++) {
                if (!arg->files->files[i].serviced) {   // found and unserviced file, so we grab it 
                    canService = 1;
                    arg->files->files[i].serviced = 1;
                    currentFile = fopen(arg->files->files[i].filename, "r+");
                    serviceCount++;
                    break;  // go and work on this file 
                }

                /** Thread couldn't find anything to work with, so they will log and exit */
                else if (i == arg->files->totalFileCount-1) {    
                    pthread_mutex_lock(&arg->service_lock); // write out to the serviced.txt
                        FILE* serviceFile = fopen(servicetxtfile, "a"); 
                        printf("Thread %X serviced %d, files. \n", (int)pthread_self(), serviceCount);
                        fprintf(serviceFile, "Thread %X serviced %d files.\n", (int)pthread_self(), serviceCount);
                        fclose(serviceFile);
                        arg->buffer->threadsFinished++;     /** need this to know when resolvers should end */
                        pthread_cond_broadcast(&arg->buffer->isEmpty); 
                    pthread_mutex_unlock(&arg->service_lock);
                    pthread_mutex_unlock(&arg->files->file_lock);
                    pthread_exit(NULL);
                }
            }
        pthread_mutex_unlock(&arg->files->file_lock);
        char* input;
        size_t len = 0;
        pthread_mutex_lock(&arg->buffer->buffer_lock);          // Try to get into the buffer , blocks if you cant
            while(getline(&input, &len, currentFile) != -1){
                    if (arg->buffer->currentPosition == ARRAY_SIZE) {   // if it ends up getting filled up
                            pthread_cond_signal(&arg->buffer->isEmpty);
                            pthread_cond_wait(&arg->buffer->isFull, &arg->buffer->buffer_lock);     // if one thread hits 20 and waits, what happens if another requester obtains that lock, at this stage, resolvers should ALWAYS be the next to the lock

                            while (arg->buffer->currentPosition == ARRAY_SIZE) {   // implies that a requestor tried to do its work right after another requestor was signaled and got its buffer filled
                                pthread_cond_signal(&arg->buffer->isEmpty);
                                pthread_cond_wait(&arg->buffer->isFull, &arg->buffer->buffer_lock);
                            }
                    } 
                

                    int length = strlen(input);
                    if(input[length-1] == '\n'){
                        input[length-1] = 0;
                    }

                    if (strlen(input) < MAX_NAME_LENGTH){
                        arg->buffer->buffer[arg->buffer->currentPosition] = input;       // should always start at 0
                        arg->buffer->currentPosition++;
                    }
                    input = NULL;
            }
            pthread_cond_signal(&arg->buffer->isEmpty);   // once the last of the contents are put into the array signal
        pthread_mutex_unlock(&arg->buffer->buffer_lock);
        fclose(currentFile);
    }
 }

 void* resolverThreads(void * inputFiles){
    resolverThreadArg* arg = (resolverThreadArg*) inputFiles;
    char* resultstxtfile = arg->resultslog;    
    FILE* results = fopen(resultstxtfile, "a"); 
    char* ipString = malloc(sizeof(char) * INET_ADDRSTRLEN);
    
    while (1) {
        // Get into the buffer and try to grab something 
        pthread_mutex_lock(&arg->buffer->buffer_lock);
            if (arg->buffer->currentPosition == 0) {                            // only signal requestors once buffer gets emptied 
                if (arg->buffer->threadsFinished == arg->buffer->threadCount) { // keep all resolver threads alive until the last requestor thread 
                    pthread_mutex_unlock(&arg->buffer->buffer_lock);            // release the lock before exiting
                    pthread_exit(NULL);
                }
                pthread_cond_signal(&arg->buffer->isFull);
                pthread_cond_wait(&arg->buffer->isEmpty, &arg->buffer->buffer_lock);    //buffer is empty release lock and wait for items 
            }   

            char* domainName;
            arg->buffer->currentPosition--;  
            domainName = arg->buffer->buffer[arg->buffer->currentPosition];  // get the next avaiable item 
        pthread_mutex_unlock(&arg->buffer->buffer_lock);

        // writing the value to results.txt
        pthread_mutex_lock(&arg->results_lock);
            if (dnslookup(domainName, ipString, INET_ADDRSTRLEN) == UTIL_FAILURE ) {
                    fprintf(results,"%s,\n", domainName);
            }
            else {
                fprintf(results,"%s,%s\n", domainName, ipString);
            }
        pthread_mutex_unlock(&arg->results_lock);
    }
    fclose(results);
    pthread_exit(NULL);
 }

// need to create the .h file because the default make file is looking for it
// <# requester> <# resolver> <requester log> <resolver log> [ <data file> ... ]
int main(int argc, char* argv[]) {
    /* timer */
    struct timeval begin, end;
    gettimeofday(&begin, NULL);

    /** parsing the input */
    int requesterThreadsCount = atoi(argv[1]);
    int resolverThreadCount = atoi(argv[2]);
    char* requesterlog = argv[3];
    char* resolverlog = argv[4];

    int ret = clearLogs(requesterlog, resolverlog);
    if (ret == -1){     // bad output file path
        exit(0);
    }

    if (argc < 6 || requesterThreadsCount < 1 || resolverThreadCount < 1) {
        printf(stderr, "Not enough command line arguments. Stopping program.\n");
        return -1;
    }
    if (requesterThreadsCount > MAX_REQUESTER_THREADS || resolverThreadCount > MAX_RESOLVER_THREADS) {
        printf(stderr, "You are requesting to many threads. Stopping program\n");
        return -1;
    }

    /** Array of threads*/
    pthread_t reqWorkers [requesterThreadsCount];
    pthread_t resWorkers [resolverThreadCount];

    /** Initializing the inputFiles struct */
    inputFiles* input_files;  
    input_files = malloc(sizeof(file)*argc-5);
    input_files->currentFileIndex = 0;
    int fileCount = 0;
    for (int i = 5; i < argc; i++) {
        file file;
        file.filename = argv[i];
        file.serviced = 0;
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
    shared->threadCount = requesterThreadsCount;
    shared->threadsFinished = 0;
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

    if (fileCount > 0 && fileCount >= MAX_INPUT_FILES){
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
    }
    
    /** Cleanup all the memory you created */
    pthread_mutex_destroy(&shared->buffer_lock);
    pthread_mutex_destroy(&input_files->file_lock);
    pthread_mutex_destroy(&resolver.results_lock);
    pthread_mutex_destroy(&requester.service_lock);
    pthread_cond_destroy(&shared->isEmpty);
    pthread_cond_destroy(&shared->isFull);
    free(shared); 
    free(input_files);

    /** stop the time tracker */
    gettimeofday(&end, NULL);
    double seconds = (end.tv_sec - begin.tv_sec) + ((end.tv_usec - begin.tv_usec)/1000000.0);  /* convert it into seconds */
    printf("./multi-lookup: total time is %f seconds\n", seconds);

    /** exit the program */
    exit(1);
}