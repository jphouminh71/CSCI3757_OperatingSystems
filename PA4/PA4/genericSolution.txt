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
#define MAXWINDOWSIZE 10 /* The maxiumum number of unique frames that each process can have in buffer */

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
typedef struct Entry {
    int pageNumber; 
    Entry* prev; 
    Entry* next;  
} Entry; 

typedef struct LL {
    Entry* head; 
    Entry* tail;
} LL;

typedef struct Process {
    int processNumber; 
    int windowSize;
    LL entries;         // linked list to store all of the entry requests that this process makes     
} Process; 

void pageit(Pentry q[MAXPROCESSES]) { 

    /* Static vars */
    static int initialized = 0;
    static int tick = 1; // artificial time
    static int timestamps[MAXPROCESSES][MAXPROCPAGES];
    static Process p[MAXPROCESSES];
    static int totalFramesNeeded;       // everytime we calculate the number of frames needed for each process check it to make sure its smaller than available physical pages 

    /* Local Vars */
    

    /* initialize static vars on first run */
    if(!initialized){	
	    initialized = 1;
        
        /* Initialize linked list for keeping track of processes */
        for (int i = 0; i < MAXPROCESSES; i++) {
            p[i].processNumber = i; 
            p[i].windowSize = 5;
            p[i].entries.head = NULL;
            p[i].entries.tail = NULL;
        }
    }

    /* Local vars */
    int proc;
    int pc;
    int page;
    int oldpage; 
    int windowSize;     // variable that will set the window size for each process
    
    for(proc=0; proc<MAXPROCESSES; proc++) { 
        /* Is process active? */
        if(q[proc].active) {
            /* Dedicate all work to first active process*/ 
            pc = q[proc].pc; 		        // program counter for process
            page = pc/PAGESIZE; 		// page the program counter needs
            timestamps[proc][page] = tick;            // update timestamp for the current page 

            /* log for that process */
            Entry e;
            e.pageNumber = page;
            e.next = NULL;
            e.prev = NULL;
            if (p[proc].entries.head == NULL) { // add to the head of linked list 
                p[proc].entries.head = &e;
                p[proc].entries.tail = &e;
            } else {    // hook up the tail to this node and update the tail of this one and the next for the current tail 
                p[proc].entries.tail->next = &e; 
                e.prev = p[proc].entries.tail;
                p[proc].entries.tail = &e; 
            }
            
            /* Calculate the Window Size for that Process and then add it to the running total of needed frames */



            /* Pagein all the needed pages for that process */

        }
    } 
    /* advance time for next pageit iteration */
    tick++;
} 
