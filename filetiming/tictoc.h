#ifndef TICTOC_H
#define TICTOC_H

#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

struct TicToc {
    struct timeval* times;
    char ** msg;
    int thread_ID;
    int cur_item;
    int max_events;
};


/**
 * Creates a TicToc timer belonging to a non-negative thread_ID or -1 if you 
 * don't have any threads.
 *
 * TicToc allocated resources must be freed by calling TicToc_free(tictoc).
 * 
 * max_events     maximal number of events to be recorded
 */
struct TicToc TicToc_create(int thread_ID, int max_events);



/**
 * Starts/resets the measurement(s)
 */
void TicToc_start(struct TicToc * t);


/**
 * Measures timing to an accuracy of about 5 microseconds (per call)
 *
 * message      String identifying the measurement point
 */
void TicToc_measure(struct TicToc * t, char * msg);



/**
 * Prints a default summary of all measurement times 
 */
void TicToc_print_summary(struct TicToc * t);



/**
 * Frees all allocated resources of this struct TicToc.
 * 
 * Don't forget to call free() on the struct
 */
void TicToc_free(struct TicToc * t);


#endif
