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
    static int tick = 1; // artificial time
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

                    // page out all the previous pages that this process had 
                    for (int i = 0; i < MAXPROCPAGES; i++) {
                        pageout(proc, i);
                    }
                    
                    // page the page that the process wanted initially
                    pagein(proc, page);    

                    // start to check to see if you can predict the next incoming pages
                    if (previousAccess[proc] == 3 && page == 0) { // single for loop 
                        for (int i = 0; i < 4; i++){
                            pagein(proc, i);
                        }
                        previousAccess[proc] = 3;
                        break;
                    }
                    else if (previousAccess[proc] == 8 && page == 0) { // double for loop
                        for (int i = 0; i < 9; i++){
                            pagein(proc, i);
                        }
                        previousAccess[proc] = 8;
                        break;
                    }
                    else if ((previousAccess[proc] == 13 && page == 8) || (previousAccess[proc] == 8 && page == 13) ) { // probabalistic loop
                        for (int i = 9; i < 14; i++){
                            pagein(proc,i);
                        }
                        previousAccess[proc] = 13;
                        break;
                    }
                    else if (previousAccess[proc] == 13 && page == 0) {  // trying to predict the linear part of the probabalistic loop 
                        for (int i = 0; i < 13; i++){
                            pagein(proc, i);
                        }
                        previousAccess[proc] = 13;
                        break;
                    }
                    previousAccess[proc] = page;
                }
            }
        }
    } 
    /* advance time for next pageit iteration */
    tick++;
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
