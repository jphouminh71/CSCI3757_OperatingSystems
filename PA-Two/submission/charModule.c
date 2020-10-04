#include<linux/init.h>   
#include<linux/module.h>  // lkm init / denit

#include<linux/fs.h>      // file manipulation
#include<linux/slab.h>    // memory allocation in kernel space 
#include<linux/uaccess.h> // userspace - kernel space memory location 

MODULE_AUTHOR("Jonathan Phouminh");
MODULE_LICENSE("GPL");

#define BUFFER_SIZE 1024 
#define DEVICE_MAJOR_NUMBER 261  
#define DEVICE_NAME "charDriver"
char* kernelBuffer; 
int openCount = 0;  

ssize_t pa2_char_driver_read(struct file* pfile, char __user *buffer, size_t length, loff_t* offset) {
	printk(KERN_ALERT "------Read Function ------\n");
	int bytesLeftToRead = BUFFER_SIZE - *offset;             // amount of bytes that are left that can be read 
	int bytesToRead = length;		     		 // amount of bytes that is actually going to be read
	printk(KERN_ALERT "OFFSET BEFORE INCREMENTING: %d\n", *offset);

	if (bytesLeftToRead < length) {  // implies that user wants to read more than what is left available, just let them read the rest
		
		/* if bytes left to read is negative, then make them read 0 */
		if (bytesLeftToRead < 0) {
			printk(KERN_ALERT "At the end of the buffer\n");
			return -1;
		}
		printk(KERN_ALERT "Going to read the remaining bytes of the buffer\n");
		bytesToRead = bytesLeftToRead; 

	}
	else {
		copy_to_user(buffer, kernelBuffer + *offset, bytesToRead);
		*offset += bytesToRead;  // have to update the offset into the buffer to the next unread spot in the device buffer 
		printk(KERN_ALERT "OFFSET AFTER INCREMENTING: %d\n", *offset);
		printk(KERN_ALERT "Read %d bytes\n", bytesToRead);
		return bytesToRead;
	}
}

ssize_t pa2_char_driver_write (struct file* pfile, const char __user* buffer, size_t length, loff_t* offset) { 
	printk(KERN_ALERT "------Write Function ------\n");
	int bytesLeftToWrte = BUFFER_SIZE - *offset; 

	// make sure theres enough room in the buffer to write if we get close to the end
	if ( bytesLeftToWrte < length ) {
		printk(KERN_ALERT "Not enough memory to write %d bytes\n", length);
		return -1;
	}

	printk(KERN_ALERT "%d values will be written to the kernel buffer", length);

	copy_from_user(kernelBuffer + *offset, buffer, length);

	printk(KERN_ALERT "WRITTEN VALUES: %s\n", buffer);
	//printk(KERN_ALERT "Kernel Buffer: %s\n", kernelBuffer);

	*offset = *offset + length;
	
	printk(KERN_ALERT "Offset is now at position: %d\n", *offset);
	return length; 
}


// Open / Close called when Device file is opened
int pa2_char_driver_open (struct inode* pinode, struct file* pfile) {
	
	printk(KERN_ALERT "Character Device Driver opened.\n");
	// print to the log file that the device is opened and also print the number of this device has been opened til now
	// probably have some variable keep track of how many times this devce driver got opened 
	openCount = openCount + 1;
	printk(KERN_ALERT "Character device driver opened: %d times.\n", openCount);
	return 0; 
}
int pa2_char_driver_close (struct inode* pinode, struct file* pfile) {

	printk(KERN_ALERT "Character Device Driver closed.\n");
	printk(KERN_ALERT "Character device driver closed: %d times.\n", openCount);
	return 0;
}


/*
	Whence value meanings: 
		0 = the position is set to the value of the offset
		1 = the position is set to the current position PLUS the value of offset
		2 = the position is set to the end of the file MINUS the value of offset

*/
loff_t pa2_char_driver_seek (struct file* pfile, loff_t offset, int whence) {
	printk(KERN_ALERT "------Seek Function ------\n");
	loff_t currentOffset = pfile->f_pos;	
	switch (whence) {
	case 0:
		currentOffset = offset;
		break;
	case 1: 
		currentOffset = currentOffset + offset;
		break;
	case 2:
		currentOffset = 1024 - offset; 		// end of the buffer is the end of the file
		break;

	default:
		// should never come here
		printk(KERN_ALERT "BAD WHENCE VALUE: %d", whence);
		return -1;
	}

	// bounds checking
	if (currentOffset < 0) {
		printk(KERN_ALERT "calculated offset: %d\n", currentOffset);
		printk(KERN_ALERT "Out of Bounds Seek. Offset Remains: %d\n", pfile->f_pos);
		return -1;
	}
	if (currentOffset > BUFFER_SIZE) {
		printk(KERN_ALERT "calculated offset: %d\n", currentOffset);
		printk(KERN_ALERT "Out of Bounds seek. Offset Remains %d\n", pfile->f_pos);
		return -1;
	}

	// now update f_pos with currentOffset

	printk(KERN_ALERT "Offset is now at position: %d\n", currentOffset);
	pfile->f_pos = currentOffset;
	return currentOffset;
}


// This is essentially the available interface to device buffer.
struct file_operations pa2_char_driver_file_operations = {

	.owner = THIS_MODULE,
	.open = pa2_char_driver_open, 
	.release = pa2_char_driver_close, 
	.read = pa2_char_driver_read, 
	.write = pa2_char_driver_write,
	.llseek = pa2_char_driver_seek
};


static int pa2_char_driver_init(void) {
	
	printk(KERN_ALERT "Inside init modules, now registering character driver\n");
	int registered = register_chrdev(DEVICE_MAJOR_NUMBER, DEVICE_NAME, &pa2_char_driver_file_operations); // returns 0 on success

	// check to make sure the device was able to be added into the kernel modules
	// double check with lsmod (lists out all of the available kernel modules)
	if (registered < 0) {
		printk(KERN_ALERT "Device Driver was unable to be registered\n");
		return registered;
	}
	printk(KERN_ALERT "Character driver was registered into the kernel modules\n");
	//printk(KERN_ALERT "Creating kernel buffer\n");
	kernelBuffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
	printk(KERN_ALERT "-------------");
	return 0;
}

static void pa2_char_driver_exit(void) {
	printk(KERN_ALERT "Inside the cdriver deinit, now deregistering character driver\n");
	unregister_chrdev(DEVICE_MAJOR_NUMBER, DEVICE_NAME);
	printk(KERN_ALERT "Character driver was unregistered from the kernel modules.\n");
	//printk(KERN_ALERT "Deallocating kernel buffer.\n");
	kfree(kernelBuffer);
}


module_init(pa2_char_driver_init);
module_exit(pa2_char_driver_exit);


/* office hour questions 
	1. *offset is the offset to the kernel space buffer right?
	2. if we are incmrementing the offset everytime we write to buffer, then are we not losing the previous read point when we enter the read function? Because the current offset would
	would be at the end of the file since the last write().

	3. I think my write is correct

	4. Confused on what the length for the read function is
		- the documentation says its the size of that buffer, is it just the number of bytes I want to copy back into the user space buffer? 
 */

/*  Side note
	1. offset is essentially just a pointer to current position to the kernel space buffer. 
	2. offset always starts at 0 upon device file open


*/
