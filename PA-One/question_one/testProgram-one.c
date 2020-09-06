#include <stdio.h>
#include <unistd.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
int main(){
         // trap table number is: 548
         int call_return_value  = syscall(548);
         printf("System call: (sys_hellworld) returns >  %d\n", call_return_value);
         return 0;
}
