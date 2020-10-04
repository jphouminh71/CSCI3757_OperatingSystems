Name: Jonathan Phouminh
Contact: joph0114@colorado.edu

How to run code: 

	1.  create subdirectory   , /kernel/modules
	2.  place the Makefile and charModule.c into the subdirectoy
	3.  compile the program with:  make -C /lib/modules/$(uname -r)/build M=$PWD
       4.  create device file with: sudo mknod -m 777 /dev/simple_character_device c 261 0
		* major number may vary across machines..
	5. load the .ko into the kernel: sudo insmod charModule.ko
	6. Place the testProgram.c into the desktop and compile it to an executable 
	7. run the executable.

	8. when finished, unload the kernel modules: sudo rmmod charModule


File Descriptions: 
        - Makefile,   this is the makefile for compiling the character device driver to ge the kernel object 
	- testProgam.c,  usespace test program to interact with the kernel module once loaded
	- charModule.c,   source code for the character device driver	





Resources: 
     malloc: https://en.wikipedia.org/wiki/C_dynamic_memory_allocation
     registering/unreg drivers: https://tldp.org/LDP/lkmpg/2.6/html/x569.html
     linuxDataStructures: https://tldp.org/LDP/tlk/ds/ds.html
     ch2OfTextbook: loading and unloading kernel modules
     inputOutputinC: https://www.tutorialspoint.com/cprogramming/c_preprocessors.htm
