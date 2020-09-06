#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/uaccess.h>
asmlinkage long sys_cs3753_add(int firstNumber, int secondNumber, int* storeAddress)
{
  /* 
  printk(KERN_ALERT "NUMBERS TO BE ADDED: %d, %d\n", firstNumber, secondNumber);
  int sumValue = firstNumber + secondNumber;   // calculating
  *storeAddress = sumValue;                     // Storing the integer result into the address 

  printk(KERN_ALERT "SUM RESULT: %d\n", *storeAddress );   // printing out the contents of the memory location , not just the computed result
  return sumValue;

  */

  // you would only use copy_from_user() when you need to 
  // actually access that pointer's value since it's in the user space
  int sumValue = firstNumber + secondNumber;
  int r =  copy_to_user(storeAddress, &sumValue, 4);   // int = 4 bytes 
  printk(KERN_ALERT "NUMBERS TO BE ADDED: %d, %d\n", firstNumber, secondNumber);  printk(KERN_ALERT "SUM RESULT: %d\n", sumValue);
  return r;

}
