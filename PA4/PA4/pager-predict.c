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

/* METHOD: Leveraging given code types and examining previously accessed pages for that process to try and classify its program type 
        examining R graphs to determine the page numbers that each program utitlizes: seed 512 
    List of program types and their page orders : (page = PC / PAGESIZE)
        1. Linear 
            Accesses these pages in order [0, 1, 2, 3, 4 ... 15]
        2. Single Loop
            Possible pages: [0, 1, 2, 3, 4]
            Accesses them in this order: [0,1,2,3,4,0,4,0,4,0,4] 
        3. Loop with Inner Branch
            [0,1,2,3,4,5,6,...,12]
            Accesses them in this order: []
        4. Double Nested Loop
            Possible pages: [0,1,2,3,4..9]
            Accesses them in this order: []
        5. Probabilistic Backward Branch
            Possible pages: [0,1,2,3,4,5,6,..14]
            Accesses them in this order: []

        Try to identify the type of page that its accessing by looking at its program counter 

*/

/* 
    How you are going to track specific pages and whats being accessed. 
        Always run the seed for the lru implementation at 512.
            1. Make sure that the order of the processes being executed are always being executed in the same order 

            2. Identify which processes are what type of program so you can get the data of the pages on them so you can predict what pages you want to add in

            3. Identify all 5 types of programs 
            




*/
/* keeping track of the entry number by keeping track of its page number */
typedef struct Process {
    int processNumber; 
    int entryCount;
    int entries[MAXPROCPAGES];         // linked list to store all of the entry requests that this process makes     
} Process; 

void pageit(Pentry q[MAXPROCESSES]) { 

    /* Static vars */
    static int initialized = 0;
    static int tick = 1; // artificial time
    static int timestamps[MAXPROCESSES][MAXPROCPAGES];
    static Process* pp = NULL; 

    /* Local Vars */
    

    /* initialize static vars on first run */
    if(!initialized){	
	    initialized = 1;
        pp =  (Process*)malloc(sizeof(Process) * (2*MAXPROCESSES));   //  40 for each process 
        for (int i = 0; i < MAXPROCESSES*2; i++) {    /* Initalize all the first entry of each process to PC = 0*/
            Process p; 
            p.processNumber = i;
            p.entryCount = 1;  
            p.entries[i] = 0;
            pp[i] = p; 
        }
    }


    /* Local vars */
    int proc;
    int pc;
    int page;
    int oldpage; 
    
    for(proc=0; proc<MAXPROCESSES; proc++) { 
        /* Is process active? */
        if(q[proc].active) {
            /* Dedicate all work to first active process*/ 
            pc = q[proc].pc; 		   
            page = pc/PAGESIZE; 		
            timestamps[proc][page] = tick;             

            /* log the page  for that process */
            pp[proc].entries[pp[proc].entryCount] = page;
            pp[proc].entryCount++;
            

            
            /* Try to determine what kind of program it is */



            /* Pagein all the needed pages for that process */

        }
    } 
    /* advance time for next pageit iteration */
    tick++;
} 


// use this to check the entries for each process
// printf("All Initalized Processes\n");
    // for (int k = 0; k < MAXPROCESSES; k++) {
    //     printf("Process ID: %d\n", pp[k].processNumber );
    //     printf("Process Entries--\n");
    //     for (int j = 0; j < pp[k].entryCount; j++) {
    //         printf("%d\n", pp[k].entries[j]);
    //     }
    //     printf("------\n");
    // }
    // exit(0);    // kill the process 