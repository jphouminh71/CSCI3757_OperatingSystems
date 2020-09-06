#include <stdio.h>
#include <stdlib.h>


void modifyPtr(int* givenPointer){
   // print out the address
   printf("%p\n", givenPointer); 

   // try to print out the contents
   printf("%d\n", *givenPointer);

   // try to modify the pointer
   *givenPointer = 10;
}

int main() {
   int* myptr = malloc(1 * sizeof(*myptr)) ;  // block for integers upto 256
   printf("%p\n", myptr);
   modifyPtr(myptr);

   printf("MODIFIED VALUE: %d\n", *myptr);
   return 0;
}
