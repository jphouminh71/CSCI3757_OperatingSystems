#include <stdio.h>
#include <unistd.h>
#include <linux/kernel.h>
#include <sys/syscall.h>

/*                                        TEST PROGRAM:
   This is the currently the way you did it that works
         Steps that this function needs to take
            1. Declare two numbers
            2. Find a spot in memory
            3. make the system call (trap #: 549)
                pass in the numbers and address of where its going to saved 
            4. retrive the system calls return value 
                it should just be the result of the addition shouldn't be any memory issues. 
            5. print out to the terminal the added value

   This is the way you should do it 
           1. Declare the firstnumber, secondnumber, and an uninitialized int for the sum 
           2. Use copy_from_user()  to get those items into the kernals memory space
           3. Make the system call
           4. in the system call fuction, retrive the memory (figure out which one)
               - make sure to log the result 
           5. compute and then do copy_to_user to move the computation back to user space memory
                 
*/

int main(){
         /*  this code works, but its not the proper way to do it? 
         // declaring the numbers and allocating memory 
         int firstNumber = 1; 
         int secondNumber = 2;

         // two way of passing a memory address for the sum to be stored  
         int* ptr = malloc(1 * sizeof(*ptr)); // allocated memory for 1 integer through the heap 


         // Making the Sys_Call. Trap #549
         int call_return_value  = syscall(549, firstNumber, secondNumber, &sum);
         printf("System call: (cs3753_add) returns >  %d\n", call_return_value);
         return 0;
         */ 

	int firstNumber = 5; 
 	int secondNumber = 10; 
        int sum = 0;

        int returnvalue = syscall(549, firstNumber, secondNumber, &sum);  // should return 0  
	printf("System Call: (cs3753_add) returns > %d\n", returnvalue);
	printf("Sum: %d\n", sum); 
}
