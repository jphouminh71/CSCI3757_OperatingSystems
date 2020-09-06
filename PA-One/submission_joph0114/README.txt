Contact Information: 
   name: Jonathan Phouminh
   email: joph0114@colorado.edu

Files / Paths: 
   cs3757_add.c 
     -info: This file contains the system function for adding and storing the result of two numbers. There is a large commented portion of code that I had in my first attempt that worked, but I tried a better solution below. 

     -path: /home/kernel/linux-hwe-4.15.0/arch/x86/kernel
     
     -instructions: Just compile the test program and run the executable. The two values that need to be added are hardcoded into source code, so just update the values if you want to change it. 
  
   helloworld.c 
     -info: This file containts the system function for printing messages into the systemlog. It is also messaging "YOU ADDED THIS LINE" because I was testing itand didn't want to recompile the whole kernel just to delete one message log. Hope thats okay. 
     -path: /home/kernel/linux-hwe-4.15.0/arch/x86/kernel

     -instructions: Just build the file and execute and the kernal outpul will be in the system logs. 
   
   testProgram-one.c 
      -info: this is the source code for test file for the helloworld system call

   testprogram-two.c 
      -info: this is the source code for test file for the csci3757_add system call 
