#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "multilookup.h"



// It is passed the struct and that allows me to have Essentially adding into the array
void *ParseFile(SharedVariables* vars){
    printf("Parse File\n");
    char *input = NULL;

    pthread_mutex_lock(&vars->fileLock);
    char *Filename = vars -> files[vars->amtFiles];
    vars -> amtFiles = vars -> amtFiles + 1;
    FILE * File = fopen(Filename,"r");
    pthread_mutex_unlock(&vars->fileLock);

    size_t len = 0;
    if(File == NULL){
        printf("Error can not open file returning \n");
        exit(1);
    }
    while(getline(&input, &len, File) != -1){
        
        //Un able to add more until resolver finishes at least one more
        if(vars->actual == 20){
            pthread_cond_wait(&vars->buffer_wait, &vars->buffer_lock);
        }
        if(vars->actual >= 1){
            printf("Tyring to signal back to resolver\n");
            pthread_mutex_lock(&vars -> SharedLock);
            pthread_cond_broadcast(&vars -> buffer_wait);
            pthread_mutex_unlock(&vars->SharedLock);
        }

        int strLength = strlen(input);
        if(input[strLength-1] == '\n'){
            input[strLength-1] = 0;
        }

        pthread_mutex_lock(&vars -> SharedLock);
        //printf("This is the line that was read: %s \n", input);
        vars -> buffer[vars -> actual] = input; //The length of the site + the \n in reality it is buffer[i].length + 1 
        vars -> actual = vars -> actual + 1;
        vars -> index = vars -> index +1;
        //printf("This is the current index of buffer in the Requester thread %d \n", vars ->index);
        //printf("Number in buffer %d \n", vars -> actual);
        input = NULL;
        pthread_mutex_unlock(&vars->SharedLock);

    }
    free(input);
    fclose(File);
    vars -> requestFinished = vars -> requestFinished + 1; // signals to resolver to break once finished
    if(vars -> requestFinished == vars -> end){

        pthread_mutex_lock(&vars -> SharedLock);
        pthread_cond_broadcast(&vars -> buffer_wait);
        pthread_mutex_unlock(&vars->SharedLock);
        printf("DONE ADDING TO BUFFER SHOULD TRY TO KILL REQUEST THREADS AND JUST DID A BROADCAST\n");
        printf("NUMBER OF FILES SERVICED: %D\n", vars -> requestFinished);
    }
    pthread_mutex_unlock(&vars->SharedLock);
    
    return NULL;
}


void * DNSoutput(SharedVariables* vars){
    printf("DNSOutput\n");
    //needs to wait for requestor to add
    while (vars->requestFinished != vars -> end || vars->actual >0){
        //printf("This is num of files serviced, %d\n", vars ->requestFinished);
        if(vars -> end == vars -> requestFinished && vars -> actual == 0){
            break;
        }

        // Ensures there is something in buffer 
        if(vars -> actual == 0 && vars->requestFinished != vars -> end){
            printf("%p Waiting in Resolve for buffer to be filled\n", pthread_self());
            printf("Files that have been serviced so far %d \n ", vars -> requestFinished);
            pthread_cond_wait(&vars->buffer_wait, &vars->buffer_lock);
            printf("%p Just finished the waiting in the Resolve function for buffer to be filled\n", pthread_self());
        }
        pthread_mutex_lock(&vars -> SharedLock);
        char ipAddress[46];
        char *hostName =vars -> buffer[vars->index];
        //printf("This is the host name we are currently viewing: %s \n", hostName);
        if(dnslookup(hostName, ipAddress, sizeof(ipAddress)) == UTIL_SUCCESS){
            //printf("Valid look up. This should be the IP address %s: \n", ipAddress);
            fprintf(vars -> outputFile, "%s, %s\n",hostName, ipAddress);
        }
        else{
            fprintf(vars -> outputFile, "%s, %s\n",hostName, "");
        }
        vars -> buffer[vars->index] = NULL;
        hostName = NULL;
        vars->index = vars-> index -1;
        vars -> actual = vars -> actual -1;

        //printf("This is the current index in the resolve function: %d \n", vars ->index);
        //printf("Actual number in buffer in Resolve Function: %d \n", vars -> actual);
        pthread_mutex_unlock(&vars->SharedLock);
        
        //signal back to requester to keep adding
        
        if(vars -> actual <= 19){
            pthread_mutex_lock(&vars->SharedLock);
            pthread_cond_signal(&vars -> buffer_wait);
            pthread_mutex_unlock(&vars->SharedLock);
        }
        
    }
    vars -> resolveFinished = vars -> resolveFinished +1;
    if(vars -> numResolve == vars -> resolveFinished){
        printf("Am i in here\n");
        pthread_cond_broadcast(&vars->end_wait);
        return NULL;
    }
    printf("This thread will exit resolve P %p\n", pthread_self());
    pthread_mutex_unlock(&vars->SharedLock);
    return NULL;
}


int main(int argc, char* argv[])
{
    //basic initilzation
    int i;
    int x;
    int z;
    int r;
    int j;
    struct SharedVariables ex;
    ex.index = -1;
    ex.requestFinished = 0;
    ex.actual = 0;
    ex.initSignal = 0;
    ex.amtFiles = 0;
    ex.resolveFinished = 0;
    int count = 0;


    
    pthread_mutex_init(&ex.SharedLock, NULL);
    pthread_mutex_init(&ex.fileLock, NULL);

    pthread_mutex_init(&ex.buffer_lock, NULL);
    pthread_cond_init(&ex.buffer_wait,NULL);

    pthread_mutex_init(&ex.init_lock, NULL);
    pthread_cond_init(&ex.init_wait,NULL);

    pthread_mutex_init(&ex.end_lock, NULL);
    pthread_cond_init(&ex.end_wait,NULL);
    

    for (x=0; x < argc-3; x++){
        printf("File = %s\n", argv[3]);
        ex.files[count] = argv[x+3];
        printf("This is file: %s at index: %d\n", ex.files[x], count);
        count ++;
    }
    ex.end = count;
    printf("This is how many files need to be SERVICED %d\n", ex.end);
    ex.outputFile = fopen("outputFile", "w");
    //printf("This is the file location %s \n", ex.files[0]);
    //printf("Now we are going to create the thread to parse the file\n");
    ex.numRequest = atoi(argv[1]);
    ex.numResolve = atoi(argv[2]);
    printf("numResolve = %d, numRequest = %d\n", ex.numResolve, ex.numRequest);
    printf("NOW BEGIN EXECUTION \n");
    
    pthread_t *WorkingThread[ex.numRequest], *ResolverThread[ex.numResolve];
    for(j = 0; j <ex.numResolve; j++){
        ResolverThread[j] = (pthread_t*) malloc(sizeof(pthread_t));
        pthread_create(ResolverThread[j], NULL, (void*)DNSoutput, &ex);
    }
    for (i = 0; i < ex.numRequest; i++){
        WorkingThread[i] = (pthread_t*) malloc(sizeof(pthread_t));
        pthread_create(WorkingThread[i], NULL, (void*)ParseFile, &ex);
    }

    for(z = 0; z < ex.numRequest; z++){
        pthread_join(*WorkingThread[z], 0);
        printf("Killed Requestor thread\n");
    }
    for(r = 0; r < ex.numResolve; r++){
        pthread_join(*ResolverThread[r], 0);
        printf("killed Resolver Thread\n");
    }
    //pthread_join(*WorkingThread, 0);
    //pthread_join(*ResolverThread, 0);
    
    
    fclose(ex.outputFile);
    printf("Ending the program\n");


    /* This block of code is for me to validate im putting shit it
    
    pthread_cond_wait(&ex.cond1, &ex.mutex); 
    int condition = ex.actual;
    for (i; i < condition; i++){
        printf("This is index: %d, and what is in buffer: %s", i, ex.buffer[i]);
    }
    printf("\n");
    printf("This is what the array buffer size is: %d \n", ex.actual);
    ex.buffer[0] = NULL;
    printf("Im putting NULL in index 0 %s\n", ex.buffer[0]);
    printf("Index 1 should still be the same %s", ex.buffer[1]);
    pthread_join(*WorkingThread, 0);
    */



    /*
    pthread_t *Thread_group = malloc(sizeof(pthread_t)*10); //gives two threads
    for (i; i < 10; i++){
        pthread_create(&Thread_group[i], NULL, counting, NULL);
    }
    for (i = 0; i < 10; i++){
        pthread_join(Thread_group[i], NULL);
    }
    */
   

    return 0;
}
