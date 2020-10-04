#include <stdio.h>    // io library
#include <unistd.h>
#include <pthread.h>  // POSIX threading library
#include <stdlib.h>
#include <sys/types.h>

int value = 0;
void *runner(void *param);  /* entry point for the threaded process */


int main(int argc, char* argv[]) {

    int updateValue = 5;

    pid_t pid;  /* processID for child process */

    /* Creating the thread */
    pthread_t tid;
    pthread_attr_t attr; 

    /* forking the main process */
    pid = fork(); 

    if (pid == 0) {  /* Child process */
        /* init the thread */
        pthread_attr_init(&attr);
        pthread_create(&tid, &attr, runner, &updateValue);
        pthread_join(tid, NULL);
        printf("CHILD: value = %d\n", value); 
    }

    else if (pid > 0) {/* Parent process tasks */
        wait(NULL);   /* wait for its child process */
        sleep(2);
        printf("PARENT: value = %d\n", value);
    }
}

void *runner(void* param) {
    /* Remember that all thread processes share there processes global / static variables */
    /* Static variables value persist across all processes */
    value = *(int*) param; 
    pthread_exit(0);
}

/* Ask TA, is it common for multi-process programs to also be multithreaded? Would you at most only want to fork once since 
    forking clones all existing threads and don't they end up running eachother over? 
 */