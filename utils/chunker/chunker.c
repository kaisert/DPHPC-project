#ifndef CHUNKER_C
#define CHUNKER_C

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

typedef unsigned long int uint64_t;

FILE * chunker_file;
char* chunker_buf;
uint64_t chunker_filesize;

void chunker_init(const char* filename) {

    FILE* fp = fopen(filename, "r");
    if(!fp) {
        perror("Chunker: File opening failed with ");
        exit(1);
    }

    fseek(fp, 0L, SEEK_END);
    chunker_filesize = ftell(fp);
    fseek(fp, 0L, SEEK_SET);


    chunker_file = fp;


    //printf("Chunker: file size is %ld\n", chunker_filesize);
    chunker_buf = (char*) calloc (chunker_filesize+1 ,sizeof(char) );

    if (!chunker_buf) {
        perror("Not enough memory to read the complete file into memory.\n");
        exit(1);
    }
}

void chunker_load_file_content() {
    uint64_t size = 0;
    uint64_t c;

    while ((c = fread(chunker_buf, sizeof(chunker_buf[0]), chunker_filesize, chunker_file)) != 0) {
        size += c;
    }
    
    if (chunker_filesize != size) {
        printf("Warning: Actual amount of data read does not match computed chunker_filesize.");
    }
}

/**
 * Divides the file into n_chunks chunks and computes the chunk start and end positions.
 *
 * n_chunks      {int}   Number of chunks or -1 for the machine's core number
 * returns      {char**} Array of char* of size n_chunks+1, indicating the boundaries
 *                       and for -1 changes the value of n_chunks to the actual number.
 *
 * Warning:  Empty chunks (start == end) are possible in rare cases
 */
char** chunker_compute_chunks(int * n_chunks) {
    int num_chunks = *n_chunks;

    int num_threads = omp_get_num_procs();
    if (num_chunks < 1)
    {
        *n_chunks  = num_threads;
        num_chunks = num_threads;
    }

    char ** chunks = (char **) calloc(num_chunks+1, sizeof(char*));
    chunks[0] = chunker_buf;
    chunks[num_chunks] = chunker_buf + chunker_filesize + 1; //last chunk ends at EOF+1


    long chunk_size = chunker_filesize/num_chunks;
    if (chunker_filesize % num_chunks != 0)
    {
        ++chunk_size;
        printf("Chunker: chunk_size has been ceiled to %ld to read the whole file.\n", chunk_size);
    } else
        printf("Chunker: chunk_size is %ld.\n", chunk_size);


    chunker_buf[chunker_filesize] = '<'; //prevent chunk computation from overshooting
    
    int i;
    if (num_chunks/num_threads > 200)
    {
        #pragma omp parallel for \
                    schedule(static,num_chunks/num_threads)
        for (i=1;i < num_chunks; ++i)
        {
            char * locbuf = chunker_buf + chunk_size * i;
            while (*locbuf++ != '<')
                /*no other work necessary*/;

            --locbuf; //reset to position of '<'

            //printf("locbuf of chunk %i set to %ld in thread %d\n", 
            //        i, (long) (locbuf - chunker_buf), omp_get_thread_num());
        }
    } else {
        for (i=1;i < num_chunks; ++i)
        {
            char * locbuf = chunker_buf + chunk_size * i;

            while (*locbuf++ != '<') { /*no other work necessary*/; }

            --locbuf; //reset to position of '<'

            //printf("locbuf of chunk %i set to %ld.\n", 
            //        i, (long) (locbuf - chunker_buf));
        }
    }

    return chunks;
}

void chunker_print_chunk(int i, char* start, char* end) {
    printf("\n==== start chunk %d ====\n", i);
    do
    {
        putchar(*start);
    } while(start++ != end);
    printf("\n==== end chunk %d ====\n", i);
}


void chunker_cleanup() {

    free(chunker_buf);

    if (ferror(chunker_file))
        puts("Chunker I/O error while reading file ");
 
    fclose(chunker_file);
}

#endif
