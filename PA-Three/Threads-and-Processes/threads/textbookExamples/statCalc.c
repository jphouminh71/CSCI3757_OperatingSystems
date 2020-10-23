#include <stdio.h>    // io library
#include <unistd.h>
#include <pthread.h>  // POSIX threading library
#include <stdlib.h>
#include <sys/types.h>

#define MAX_ARRAY_SIZE 7

/* global variables */
int meanValue = 0; 
int minimumValue = 0; 
int maximumValue = 0;  

void *calcMean(void *param);
void *calcMax(void *param);
void *calcMin(void *param);


int main(int argc, char* argv[]) {

    printf("%s\n", argv);
    return 0;



    /* creating the threads */
    pthread_t tid, tid2, tid3;
    pthread_attr_t attr, attr2, attr3;

    pthread_attr_init(&attr);
    pthread_attr_init(&attr2);
    pthread_attr_init(&attr3);

    pthread_create(&tid, &attr, calcMean, &argv);
    pthread_create(&tid2, &attr2, calcMax, &argv);
    pthread_create(&tid3, &attr3, calcMin, &argv);



    pthread_join(tid, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL); 


    printf("The average value is %d\n", meanValue);
    printf("The minimum value is %d\n", minimumValue);
    printf("The maximum value is %d\n", maximumValue);

    return 0;
}

void *calcMean(void *param) {
    int fvalues = *(int*) param;
    double balance[5] = {1000.0, 2.0, 3.4, 7.0, 50.0};
    printf("%f\n", balance[0]);
}
void *calcMax(void *param) {
    int values = *(int*) param;
    printf("%d\n", values);
}
void *calcMin(void *param) {
    int values = *(int*) param;
    printf("%d\n", values);
}