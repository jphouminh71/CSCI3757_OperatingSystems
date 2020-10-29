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

    fprintf(serviceFile, "%s", "");
    fprintf(resultsFile, "%s", "");

    fclose(serviceFile);
    fclose(resultsFile);
}


void testDNS() {

    /* FOR RIGHT NOW ONLY, just create dummy test strings so you can figure out how to the dns function */
    char* testString1 = "facebook.com";
    char* testString2 = "youtube.com";
    char* testString3 = "yahoo.com";
    char* ipString = malloc(sizeof(char) * INET_ADDRSTRLEN);

    // grab an item from the shared array then do this 
    if (dnslookup(testString3, ipString, INET_ADDRSTRLEN) == UTIL_FAILURE ) {
        // we take the domain name we couldn't find and change ipString to the domain name + ','
    }

    printf("IPSTRING: %s\n", ipString);  // should print out the generated ip string

    free(ipString);
}


void* requesterThreads(void * inputFiles){
    requestThreadArg* arg = (requestThreadArg*) inputFiles;
    char* servicetxtfile = arg->servicelog;
    FILE* serviceFile = fopen(servicetxtfile, "a");  // append

    bool canService = true;
    FILE* currentFile;
    char buff[MAX_NAME_LENGTH];
    int serviceCount = 0;

    while(canService) {
        int i = 0;
        canService = false;

        /** The thread will look through list of unprocessed files */
        pthread_mutex_lock(&arg->files->file_lock);  /** Only single requestor thread may search through list of files  */
            for (i = 0; i < arg->files->totalFileCount; i++) {
                if (!arg->files->files[i].serviced) {   // found and unserviced file, so we grab it 
                    canService = true;
                    arg->files->files[i].serviced = true;
                    currentFile = fopen(getExtension(arg->files->files[i].filename), "r+");
                    serviceCount++;
                    break;  // go and work on this file 
                } 
            }
        pthread_mutex_unlock(&arg->files->file_lock);

        if (!canService) { break; } /** if thread can't find anything to service then exit */
 
        int currentLine = 0;
        char* input;
        size_t len = 0;
        while(getline(&input, &len, currentFile) != -1){
            pthread_mutex_lock(&arg->buffer->buffer_lock);     // aquire lock into the buffer 
                if (arg->buffer->currentPosition == ARRAY_SIZE) {   // Will tell resolvers to work if the buffer is full or no more threads have work? 
                    pthread_cond_signal(&arg->buffer->isEmpty);         // if the resolvers are waiting then we tell them something is here
                    pthread_cond_wait(&arg->buffer->isFull, &arg->buffer->buffer_lock);
                }
                printf(">%d\n", currentLine);
                currentLine++;

                int length = strlen(input);
                char* domainName = (char *)malloc(length-1);
                if(input[length-1] == '\n'){
                    int i = 0;
                    for (i = 0; i < length-1; i++){
                        domainName[i] = input[i];
                    }
                    input = domainName;
                }
                printf("ADDING DOMAIN: %s\n", input);
                arg->buffer->buffer[arg->buffer->currentPosition] = input;       // should always start at 0
                arg->buffer->currentPosition++;
                input = NULL;
                domainName = NULL;
                free(domainName);
            pthread_mutex_unlock(&arg->buffer->buffer_lock);
        }
        fclose(currentFile);
    }
    printf("You should see me before resolvers finish\n");
    // By this point, you should have put every domain name in the file and you are just doing the printing
    pthread_mutex_lock(&arg->service_lock);
        fprintf(serviceFile, "Thread %X serviced %d files.\n", (int)pthread_self(), serviceCount);
    pthread_mutex_unlock(&arg->service_lock);
    fclose(serviceFile);
    pthread_exit(NULL);
 }

 void* resolverThreads(void * inputFiles){
    resolverThreadArg* arg = (resolverThreadArg*) inputFiles;
    char* resultstxtfile = arg->resultslog;    
    FILE* results = fopen(resultstxtfile, "a"); 
    char* ipString = malloc(sizeof(char) * INET_ADDRSTRLEN);
    
    while (1) {
        printf(">>: %d\n", arg->buffer->threadsFinished);
        if (arg->buffer->threadsFinished == arg->buffer->threadCount) {     /** If requesters have finished, clear out the array  */
            printf("I GOT HERE!\n");
            pthread_mutex_lock(&arg->buffer->buffer_lock);   // Aquire access to the buffer
                char* domainName;
                arg->buffer->currentPosition--;  
                domainName = arg->buffer->buffer[arg->buffer->currentPosition];  // get the next avaiable item 
            pthread_mutex_unlock(&arg->buffer->buffer_lock);


            // resolver should have access to a domain name at this point 
            pthread_mutex_lock(&arg->results_lock);     // Aquire lock to the results.txt file
                if (dnslookup(domainName, ipString, INET_ADDRSTRLEN) == UTIL_FAILURE ) {
                        printf("%s,\n", domainName);
                        fprintf(results,"%s,\n", domainName);
                }
                else {
                    printf("%s,%s\n", domainName, ipString);
                    fprintf(results,"%s,%s\n", domainName, ipString);
                }
            pthread_mutex_unlock(&arg->results_lock);
            break;
        }
 
        pthread_mutex_lock(&arg->buffer->buffer_lock);   // Aquire access to the buffer
            if (arg->buffer->currentPosition <= 0) {   
                pthread_cond_signal(&arg->buffer->isFull);   // tell the requestors that buffer is empty and can 
                pthread_cond_wait(&arg->buffer->isEmpty, &arg->buffer->buffer_lock);    //buffer is empty release lock and wait for items 
                printf("I CAME BACK FROM SLEEP\n");
            }   
            char* domainName;
            arg->buffer->currentPosition--;  
            domainName = arg->buffer->buffer[arg->buffer->currentPosition];  // get the next avaiable item 
        pthread_mutex_unlock(&arg->buffer->buffer_lock);


        // resolver should have access to a domain name at this point 
        pthread_mutex_lock(&arg->results_lock);     // Aquire lock to the results.txt file
            printf("DOMAIN NAME BEFORE EVAL: %s\n", domainName);
            if (dnslookup(domainName, ipString, INET_ADDRSTRLEN) == UTIL_FAILURE ) {
                    printf("%s!\n", domainName);
                    fprintf(results,"%s!\n", domainName);
            }
            else {
                printf("%s,%s\n", domainName, ipString);
                fprintf(results,"%s,%s\n", domainName, ipString);
            }
        pthread_mutex_unlock(&arg->results_lock);
    }
    free(ipString);
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

    if (argc < 6 || requesterThreadsCount < 1 || resolverThreadCount < 1) {
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
    
    /*
    for (int f = 0; f < resolverThreadCount; f++) {
        pthread_join(resWorkers[f],NULL);
    }
    */
    
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