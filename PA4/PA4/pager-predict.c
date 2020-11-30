/*
 * File: pager-predict.c
 * Author:       Andy Sayler
 *               http://www.andysayler.com
 * Adopted From: Dr. Alva Couch
 *               http://www.cs.tufts.edu/~couch/
 *
 * Project: CSCI 3753 Programming Assignment 4
 * Create Date: Unknown
 * Modify Date: 2012/04/03
 * Description:
 * 	This file contains a predictive pageit
 *      implmentation.
 */

#include <stdio.h> 
#include <stdlib.h>

#include "simulator.h"

void pageit(Pentry q[MAXPROCESSES]) { 

    /* Static vars */
    static int initialized = 0;
    static int previousAccess[MAXPROCESSES];    // array of previously accessed pages for each process (initially 0 for all)
    

    /* initialize static vars on first run */
    if(!initialized){	
	    initialized = 1;
        for (int i = 0; i < MAXPROCESSES; i++) {
            previousAccess[i] = 0; 
        }
    }

    /* Local vars */
    int proc;
    int pc;
    int page;
    
    for(proc=0; proc<MAXPROCESSES; proc++) { 
        /* Is process active? */
        if(q[proc].active) {
            /* Dedicate all work to first active process*/ 
            pc = q[proc].pc; 		   
            page = pc/PAGESIZE; 		             

            if (!q[proc].pages[page]) {  // if the page this process is looking for isn't in the physical frames    
                if (!pagein(proc,page)){
                     
                    for (int i = 0; i < MAXPROCPAGES; i++) {
                        pageout(proc, i);
                    }

                    
                    // start to check to see if you can predict the next incoming pages
                    if (previousAccess[proc] == 3 && page == 0) { // single for loop 
                        //printf("single loop\n");
                        int k = 0;
                        for (int i = 0; i < 4; i++){
                            if (pagein(proc, i)){
                                k++;
                            }
                        }
                        previousAccess[proc] = k;
                        break;
                    }
                    else if (previousAccess[proc] == 9 && page == 0) { // double for loop
                        //printf("double loop\n");
                        int j = 0;
                        for (int i = 0; i < 9; i++){
                            if (pagein(proc, i)) {
                                j++;
                            }
                        }
                        previousAccess[proc] = j;
                        break;
                    }
                    else if ((previousAccess[proc] == 13 && page == 8) || (previousAccess[proc] == 8 && page == 13) ) { // probabalistic loop
                        //printf("probabilstic loop\n");
                        int g = 0; 
                        for (int i = 9; i < 14; i++){
                            if (pagein(proc,i)) {
                                g++;
                            }
                        }
                        previousAccess[proc] = g;
                        break;
                    }
                    else if (previousAccess[proc] == 13 && page == 0) {  // trying to predict the linear part of the probabalistic loop
                        //printf("linear p loop\n");
                        int f = 0;
                        for (int i = 0; i < 13; i++){
                            if (pagein(proc, i)) {
                                f++;
                            }
                        }
                        previousAccess[proc] = f;
                        break;
                    }
                    else if ((previousAccess[proc] == 3 && page == 11) || (previousAccess[proc] == 3 && page == 10)) {   // loop with inner branch
                        //printf("branch loop\n");
                        int y = 0;
                        for (int i = 11; i < 13; i++){
                            if (pagein(proc,i)){
                                y++;
                            }
                        }
                        previousAccess[proc] = y;
                        break;
                    }
                    else {
                        //printf("linear\n");
                        pageout(proc, page-1);
                        pagein(proc, page);
                        pagein(proc, page+1); // bring in the next page since thats what it most likely will do 
                        previousAccess[proc] = page+1;
                    }
                }
            }
        }
    } 
} 

// /* start to check to see if you can predict the next incoming pages */
// if (previousAccess[proc] == 4 && page == 4) { // single for loop 
//     for (int i = 0; i < page; i++){
//         pageout(proc, i);
//     }
//     pagein(proc, 1);
//     pagein(proc, 2);
//     pagein(proc, 3);
// }

// // if this is too many than you need to trim down the amount you will bring in 
// else if (previousAccess[proc] == 8 && page == 8) { // double for loop 
//     pagein(proc, 1);
//     pagein(proc, 2);
//     pagein(proc, 3);
//     pagein(proc, 4);
//     pagein(proc, 5);
//     pagein(proc, 6);
//     pagein(proc, 7);
//     pagein(proc, 8);
// }

// else if (previousAccess[proc] == 13 && page == 9) { // probabalistic loop 
//     pagein(proc, 9);
//     pagein(proc, 10);
//     pagein(proc, 11);
//     pagein(proc, 12);
//     pagein(proc, 13);
// }
// else if (previousAccess[proc] == 13 && page == 0){ // other part of probabilistic loop
//     for (int i = 0; i < MAXPROCPAGES; i++){
//         pageout(proc, i);
//     }
//     pagein(proc, 0);
// }
// else if (previousAccess[proc] == 4 && page == 10 ){  // loop with inner branch
//     for (int i = 0; i < page; i++){
//         pageout(proc, i);
//     } 
//     pagein(proc, 10);
//     pagein(proc, 11);
// }
// else {  // linear behavior 
//     pagein(proc, page);
//     pagein(proc, page+1);
//     pageout(proc, page-1);
// }
// /* set the previous page that you just swapped */
// previousAccess[proc] = page;
// timestamps[proc][page] = tick;


// #include <stdio.h> 
// #include <stdlib.h>

// #include "simulator.h"
// #define MAXWINDOWSIZE 10 /* The maxiumum number of unique frames that each process can have in buffer */

/* Optimal Solultion Strategy
    * attempting to implementing the solution where we allocate # of frames based on calculated window size
    * Measure size of page by defining a delta of the most recent page references for that current process
         - The working set is the set of unique pages 
         - recently accessed pages are likely to be referenced again 
         - then allocate to a process the size of the working set 
    * Once a working set has been selected computed
         - 

    * Periodicially compute a working set and working set size for each process
         - Allocate at least the size of a working set 
         - Demand D = Sum of all processes working set size, if the total amount is greater than PHYSICALPAGES 
         then swap out a process and rallocate the frames to other processes 


    Implementation Details
    

    * How you are going to keep track of when to stop adding to the window size 
       - at every iteration calculate a new window size
       - go through the 
    

    0. Keep track of all the pages that got put into the buffer and also which processes it belongs 


*/

/* keeping track of the entry number by keeping track of its page number */
// typedef struct Entry {
//     int pageNumber; 
//     Entry* prev; 
//     Entry* next;  
// } Entry; 

// typedef struct LL {
//     Entry* head; 
//     Entry* tail;
// } LL;

// typedef struct Process {
//     int processNumber; 
//     int windowSize;
//     LL entries;         // linked list to store all of the entry requests that this process makes     
// } Process; 

// void pageit(Pentry q[MAXPROCESSES]) { 

     /* Static vars */
//     static int initialized = 0;
//     static int tick = 1; // artificial time
//     static int timestamps[MAXPROCESSES][MAXPROCPAGES];
//     static Process p[MAXPROCESSES];
//     static int totalFramesNeeded;       // everytime we calculate the number of frames needed for each process check it to make sure its smaller than available physical pages 

//     /* Local Vars */
    

     /* initialize static vars on first run */
//     if(!initialized){	
// 	    initialized = 1;
        
//         /* Initialize linked list for keeping track of processes */
//         for (int i = 0; i < MAXPROCESSES; i++) {
//             p[i].processNumber = i; 
//             p[i].windowSize = 5;
//             p[i].entries.head = NULL;
//             p[i].entries.tail = NULL;
//         }
//     }

     /* Local vars */
//     int proc;
//     int pc;
//     int page;
//     int oldpage; 
//     int windowSize;     // variable that will set the window size for each process
    
//     for(proc=0; proc<MAXPROCESSES; proc++) { 
         /* Is process active? */
//         if(q[proc].active) {
             /* Dedicate all work to first active process*/ 
//             pc = q[proc].pc; 		        // program counter for process
//             page = pc/PAGESIZE; 		// page the program counter needs
//             timestamps[proc][page] = tick;            // update timestamp for the current page 

             /* log for that process */
//             Entry e;
//             e.pageNumber = page;
//             e.next = NULL;
//             e.prev = NULL;
//             if (p[proc].entries.head == NULL) { // add to the head of linked list 
//                 p[proc].entries.head = &e;
//                 p[proc].entries.tail = &e;
//             } else {    // hook up the tail to this node and update the tail of this one and the next for the current tail 
//                 p[proc].entries.tail->next = &e; 
//                 e.prev = p[proc].entries.tail;
//                 p[proc].entries.tail = &e; 
//             }
            
/* Calculate the Window Size for that Process and then add it to the running total of needed frames */



             /* Pagein all the needed pages for that process */

//         }
//     } 
     /* advance time for next pageit iteration */
//     tick++;
// } 
