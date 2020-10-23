#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

/* This program is showing you the basic creation of process creation */

int main() {

    pid_t pid;  /* variable for processid */

    /* fork a child process */
    pid = fork();
    int status = 0;  /* used by the parent process to keep track of it termianted children */

    if (pid < 0) { /* process creation failed*/
        fprintf(stderr,"Fork ailed");
        return -1; 
    }

    /* code for child process */
    else if (pid == 0) {
        //execlp("/bin/ls", "ls", NULL); /* just lists the current directories contents */
        
        /* exit with status 1*/
        printf("Child processID: %d\n", getpid());
        exit(1); 
    }

    /* code for the parent process */
    else {
        /* parent will wait for the child process to complete and hop off the ready queue */ 
        wait(&status);
        printf("%d\n",status);
        if (status == 1) {
            printf("My one child process is completed!\n");
        }
    }


}