#include <stdio.h>    // io library
#include <unistd.h>
#include <pthread.h>  // POSIX threading library
#include <stdlib.h>

/* The following program demonstrates basic Pthreads API for constructing a 
multithreaded program that calculates the summation of a non-negative integer 
in a separate a separate thread */

int sum;  /* this piece of data is shared by all thread(s)*/
void *runner(void *param); /* threads call this function */



int main(int argc, char* argv[]) {
	
	pthread_t tid;  /* the thread identifier */
	pthread_attr_t attr;  

	if (argc != 2) {
		printf("Not enough arguments.\n");
		return -1;
	}

	if (atoi(argv[1]) < 0) {
		printf("Value passed must be positive.\n");
	}

	/* get the default attributes */
	/* attributes define the thread's;  stack size / scheduling details */
	pthread_attr_init(&attr);

	/* Creating the actual thread:  threadID, entryPoint, additional parameters */
	pthread_create(&tid, &attr, runner, argv[1]);

	/* wait for the thread to exit; the second param is the retval if you wanted it*/
	pthread_join(tid, NULL);
	printf("Thread joining complete\n");

	printf("sum = %d\n", sum);


}

/* void is generic pointer type. A void* can be converted to any othe rpointer type WITHOUT an explicit cast */
/* we could have just passed it as an int, but we didn't so we just convert it inside of this function. */
void* runner(void* param) {
	printf("Another thread is executing this!\n");
	int i, upper = atoi(param);
	sum = 0;

	for (i = 1; i <= upper; i++) {
		sum += i;
	}

	/* purposely waiting 10 seconds */
	sleep(2);
	pthread_exit(0);
}
