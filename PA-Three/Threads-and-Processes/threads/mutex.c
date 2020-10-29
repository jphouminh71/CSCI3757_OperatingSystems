#include <util.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#define NUM_THREADS 5
#define BUFFER_SIZE 50

/* Program is to just play around with mutexes and figure out how they work */
/* MUTEX: 
    Short for mutual exclusion, if a thread wants to enter its CS then it must aquire the lock.
    Any thread that doesn't have access to the lock will spin until it able to get the lock.
*/


/* Structure for passing data around */
typedef struct data {
    char buffer[BUFFER_SIZE];
    int index; 
} data;

void *increment(void * counter){
    
    data* c = (data*)counter;     // casting to make it usable

    // fill the buffer with a '*'
    
    (*c).index++;
    pthread_exit(NULL);
 }

 void *decrement(void * counter)
 {
    // remove a character from the buffer '*'

    printf("Decrementing!\n");
    data*c = (data*)counter;  // casting to make it usable
    (*c).index--;

    pthread_exit(NULL);
 }

 int main (int argc, char *argv[])
 {
     data mydata;
     mydata.index = 5;

    pthread_t threads[NUM_THREADS];
    int rc;
    long t;

    for(t=0; t<NUM_THREADS-2; t++){     // 3 decrementing threads

       rc = pthread_create(&threads[t], NULL, decrement, &mydata);
       if (rc){
          printf("ERROR; return code from pthread_create() is %d\n", rc);
          return -1;
       }
    }

    for(int i=3; i<5; i++){            // 2 incrementing threads

        rc = pthread_create(&threads[i], NULL, increment, &mydata);
        if (rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            return -1;
        }
    }

    // joining all the threads
    for (int k = 0; k < NUM_THREADS; k++) {
        pthread_join(threads[k], NULL);
    }
    printf("value of counter: %d\n", mydata.index);
 }