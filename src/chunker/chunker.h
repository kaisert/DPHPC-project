#ifndef CHUNKER_H
#define CHUNKER_H

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


/**
 * Note that only one chunker can exist at any point in time: 
 * Call chunker_cleanup() to free all previously allocated resources.
 */
void chunker_init(const char* filename);


void chunker_load_file_content();



/**
 * Divides the file into n_chunks chunks and computes the chunk start and end 
 * positions. The returned array has to be freed by the callee: free(chunks)
 *
 * n_chunks      {int}   Number of chunks or -1 for the machine's core number
 * returns      {char**} Array of char* of size n_chunks+1, indicating the 
 *                       boundaries and for -1 changes the value of n_chunks 
 *                       to the actual number.
 *
 * Warning:  Empty chunks (start == end) are possible in rare cases 
 */
char** chunker_compute_chunks(int * n_chunks);


void chunker_print_chunk(int i, char* start, char* end);


void chunker_cleanup();

#endif
