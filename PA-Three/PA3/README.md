# COMPILE / RUN COMMANDS
    
    compile: 
    gcc -o run -pthread  multi-lookup.c util.c
    run: 
    ./run 1 1 serviced.txt results.txt names1.txt   , creates a single requestor thread so you have to mock up some data 




# AT 10PM: STOP EVERYTHING AND PREPARE IT ALL FOR SUBMISSION



# BUG LIST 
    * When you are given a bunch of bogus filenames, the programs freezes. FIXED
    * Segfault when number of requestor threads is >= resolver threads. 
        - signal vs broadcast?
        - find where you are accessing the buffer and print out the values of what the buffer index is. 

        1. Go uncomment all the file closes
        2. If you end up figuring this out at the end, try to free up memory.

    * The service file is is an abitrary string, its not just serviced.txt or results.txt


    * What you think the problem is.
        1. When a requestors fills in the buffer with 20 items, it will release the lock and block. The mutex is now unlocked and you expect a resolver to come and pick it up
        2. What is happening in certain cases is that once a requester unlocks the lock, another requestor obtains access to the lock, but shouldn't it recognize that the array is already filled even if it does get the lock? 



# ERROR HANDLING
    * When you get a bogus host name, print out a message to stderr saying that you got a bogus hostname
        - Do the same for bogus output / input file path 
    
    * have to catch all potential system call errors and direct them to stderr.




# Personal Notes
    * mutexes block if calling threads can't get the thread
    * use mutexes to ensure mutual exclusion when accessing shared resources
    * use condition variables for to have requester threads and resolver threads communicate with eachother
        requesters should WAIT if the sharedBuffer is full.  => signal resolvers when there is an index to grab in buffer, via broadcast()
        resolvers should WAIT if the sharedBuffer is empty.  => signal requesters when there is an index to fill in buffer, via broadcast


# Task 1 (setup) DONE 
    * Build up the structures for arguments you need to pass the resolver and requester threads
    * Get the main function set up 
        1. Parse the command line arguments , DONE
        2. Initialize the structures that are going to be passed into the threads
        3. Create the threads and pass in their arguemnts
        4. join all the threads
        5. destroy all the mutexes / condition variables

    * Make sure threads are recieving the correct information 
        1. Make sure the correct amount of threads are being created
        2. Cast the single parameter to the type of the structure
        3. Check the parameters for each type of worker thread, just loop through and print them all out.

# task 2 (Work on the requester threads)  NOT DONE
    * Ensure that each requester thread is able to access the names of the files 
    * Open each file and print out the contents for each thread
        - just make sure all the domain names are being printed out for the given passed files 
    * set up edge case for being able to open the given file
    * for the current thread, attempt to loop through the files arguments
    * for X amount of threads, make sure they process all the files, ex given 2 req threads and 4 files, make sure they get all the files
    * Once you have access to an open file, try to loop through each line, just have one thread to test this
    * start loading the shared buffer and make the threads sleep if they run out of space. STOP HERE. you need to start working on the resolver threads next
        because you need to start getting condition variables involved.

# task 3 (work on resolver threads)  NOT DONE
    * make sure your getting access to all the things you need
    * write tests so you can figure out how the dns function works
    * when you figure out how the dns function works, write the test results out to results.txt 



# THIS IS THE PROBLEM 
For some reason when you do input files 1 - 12 it works
When you do 13, it never opens up.

Heres what you are gonna try to look at tomorrow

see how many total input files are passed to the requester
and see how many input files the requester is actually getting