/*
 * File: pager-lru.c
 * Author:       Andy Sayler
 *               http://www.andysayler.com
 * Adopted From: Dr. Alva Couch
 *               http://www.cs.tufts.edu/~couch/
 *
 * Project: CSCI 3753 Programming Assignment 4
 * Create Date: Unknown
 * Modify Date: 2012/04/03
 * Description:
 * 	This file contains an lru pageit
 *      implmentation.
 */

#include <stdio.h> 
#include <stdlib.h>

#include "simulator.h"
void pageit(Pentry q[MAXPROCESSES]) { 
    /* This file contains the stub for an LRU pager */
    /* You may need to add/remove/modify any part of this file */

    /* Static vars */
    static int initialized = 0;
    static int tick = 1; // artificial time
    static int timestamps[MAXPROCESSES][MAXPROCPAGES];

    /* Local vars */
    int proctmp;
    int pagetmp;

    /* initialize static vars on first run */
    if(!initialized){
	for(proctmp=0; proctmp < MAXPROCESSES; proctmp++){
	    for(pagetmp=0; pagetmp < MAXPROCPAGES; pagetmp++){
		timestamps[proctmp][pagetmp] = 0; 
	    }
	}
	initialized = 1;
    }

    /* Local vars */
    int proc;
    int pc;
    int page;
    int oldpage;


    
    /* LRU algorithm, look back at the processes and look for the one with the most ticks 
        * Follows the idea of local replacement
        * Looks through current processes available pages and checks if they are swapped in
        * Looks for that set of pages timestamps and look for the least recently used, 
            should be the one with the timestamp greater than the current page we are trying to swap
    */
    
    for(proc=0; proc<MAXPROCESSES; proc++) { 
        /* Is process active? */
        if(q[proc].active) {
            /* Dedicate all work to first active process*/ 
            pc = q[proc].pc; 		        // program counter for process
            page = pc/PAGESIZE; 		// page the program counter needs
            timestamps[proc][page] = tick;            // update timestamp for the current page 

            // if the that current index is unallocated or the page is already in there 
            if(!q[proc].pages[page]) {

                // if page the program counter is pointing too isn't paged in 
                if(!pagein(proc,page)) {  // if the page I want isn't paged into the frames 
                
                   int maxTime = timestamps[proc][page];
                   int pageToSwap = timestamps[proc][page]%MAXPROCPAGES;  // indexes into the oldest page based on the 'tick', @22ticks means that index 2 is the oldest page
                //    printf(">> %d\n", maxTime);
                //    printf(">> %d\n", pageToSwap);
                //    printf("PAGE TO SWAP %d\n", pageToSwap);
                   
                    // look through for a page that has an older time 
                    for(oldpage=pageToSwap; oldpage > 0; oldpage--) {

                        if (q[proc].pages[oldpage] && timestamps[proc][oldpage] > maxTime ){
                                maxTime = timestamps[proc][oldpage];
                                pageToSwap = oldpage;
                        }
                    }
                    pageout(proc, pageToSwap);
                    //printf("-------\n");
                }
            }
        }
    } 
    

    /* advance time for next pageit iteration */
    tick++;
} 