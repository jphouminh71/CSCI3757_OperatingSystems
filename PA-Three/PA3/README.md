





# Personal Notes
    * mutexes block if calling threads can't get the thread
    * use mutexes to ensure mutual exclusion when accessing shared resources
    * use condition variables for to have requester threads and resolver threads communicate with eachother
        requesters should WAIT if the sharedBuffer is full.  => signal resolvers when there is an index to grab in buffer, via broadcast()
        resolvers should WAIT if the sharedBuffer is empty.  => signal requesters when there is an index to fill in buffer, via broadcast


# Task 1 (setup)
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

# task 2 (Work on the requester threads)
    * Ensure that each requester thread is able to access the names of the files 
    * Open each file and print out the contents for each thread
        - just make sure all the domain names are being printed out for the given passed files 
    * set up edge case for being able to open the given file
    * for the current thread, attempt to loop through the files arguments
    * for X amount of threads, make sure they process all the files, ex given 2 req threads and 4 files, make sure they get all the files
    * Make the threads log the hostname into serviced.txt
    


        
