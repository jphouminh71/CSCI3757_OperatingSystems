#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>  // malloc
#include <unistd.h>  
#include <linux/kernel.h>  // interacting with kernel
#include <fcntl.h> // opening and closing system files
#include <string.h>
#define DEVICE_FILE_NAME "/dev/simple_character_device"
#define BUFFER_SIZE 1024

int main() {

	char* userBuffer;
	int readLength = 0;
	int offsetValue = 0;
	int whenceValue = 0;


	// open the deviceFile program
	// driver files are represented by files in linux
	int deviceFile = open(DEVICE_FILE_NAME, O_RDWR);  // oflag is set to "open_readwrite" so we can read and write to this driver
	
	// check to see if you opened it correctly
	if (deviceFile < 0) {  
		printf("Failed to open device file.\n");
		printf("Returning %c*\n", deviceFile); 
		return deviceFile;
	}
	printf("Device file opened successfully\n");

	// now we ask for user input and begin calling 
	/* This menu is where we call the generic menu operations that our device driver supports 
		'r' = .read = pa2_char_driver_read, 
		'w' = .write = pa2_char_driver_write,
		's' = .llseek = pa2_char_driver_seek
		    Each of the operations should open the device file and then close it when its execution is finished
		      .open = pa2_char_driver_open, 
		      .release = pa2_char_driver_close,
	*/
	bool active = true;
	char choice;
	while(active){
		printf("\n");
		printf("Enter driver options\n");
		printf("r = read()\n");
		printf("w = write()\n");
		printf("s = seek(offsetValue, whence)\n");
		printf("e = terminate program\n");
		printf("\n");
		choice = getchar();
		switch (choice) {

			/* had to use memset because there was a write issue where i would write to a user buffer and
			   and it would reflect when I allocated another buffer in the read 	
			*/
			case 'r': 
				userBuffer = malloc(BUFFER_SIZE);
				memset(userBuffer, ' ',strlen(userBuffer));
				printf("Enter number of bytes you want to read.\n");
				scanf("%d", &readLength);
				
				// make sure user doesn't ask to read memory out of the buffer size
				if (readLength < 0) {
					printf("Bounds error: cannot read negative bytes");
					break;
				}
				if (readLength > BUFFER_SIZE){
					printf("Bounds error: cannot read more than 1024 bytes");
					break;
				}
				int BytesRead = read(deviceFile, userBuffer, readLength);
				printf("Total number of bytes read: %d\n", BytesRead);

				// print out the data that was read 
				printf("READ DATA: %s ", userBuffer);
				free(userBuffer);
				break;
			case 'w':
				userBuffer = malloc(BUFFER_SIZE);
				printf("Enter the data you want to write\n");
				scanf(" %[^\n]", userBuffer);
				
				if (strlen(userBuffer) > BUFFER_SIZE) {
					printf("Overflow buffer input.");
					break;	
				}
			
				write(deviceFile, userBuffer, strlen(userBuffer)); 
				free(userBuffer);
				
				break;
			case 's':
				printf("Enter value for whence.\n");
				scanf("%d", &whenceValue);
				printf("Enter value for offset.\n");
				scanf("%d", &offsetValue);
				
				int seekValue = llseek(deviceFile, offsetValue, whenceValue);
				break;
			case 'e':
				printf("terminating program\n");
				active = false;
				break;
			default:
				printf("invalid input\n");
		}
		choice = getchar();
	}
	close(deviceFile);
	return 0;
}


/* Side notes 

	- Device buffer never actually clears until you unload the module
	- whenever you use printk, make sure you have a \n at then end of it or its going to lag behind.
	- The way to interact with the device driver. The Device file is essentially whats going to be and you need to compile a test program


	Interaction between the two buffers
		- The kernel buffer is a static buffer that never gets freed until the lkm is unloaded
		- the user buffer is allocated and freed upon read and write
		
		Example for a read: 
			- When user issues a read, a new buffer will be created and filled with ' '
			- kernel will load its contents the beginning of the user buffer 
			- user buffer will then output all the contents that it got loaded with.
*/




