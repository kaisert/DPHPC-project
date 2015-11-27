#ifndef H_SEGLIST
#define H_SEGLIST

#include<stddef.h>

#define COMPRESSION_CACHE_SIZE 512;

// TYPES
typedef struct deflated_dts_struct {
    char*  start;
    char*  end;
    char*  buf_end;
    char   compression_buffer[COMPRESSION_CACHE_SIZE * sizeof(token_t)];
    compression_stream_t* compr_stream;
    struct deflated_dts_struct* next;
} dts_t;


typedef struct dts_read_iter_struct {
    dts* dts;
    token_t* current;
    off_t global_offset;
} dts_ri_t;

/*
 * allocated a new token stream buffer. if dts is not null, appends the new
 * buffer at the end of the list.
 *
 */
dts_t* dts_create_buffer(
        dts_t* dts ///
        );


size_t dts_append_n(
        dts_t* dts,
        token_t* chunk,
        size_t n);


void dts_flush( // flushes a segment
        dts_t* seg
        ); 


// the user is responsible for memory management in the case of the read iter
void dts_init_read_iter(  // creates a read iterator for the entire list
        dts_ri_t* read_iter,
        dts_t* dts
        );


size_t dts_next_n( /// returns the number of tokens written to chunk
        dts_ri_t* read_iter, /// read iterator
        token_t *chunk,      /// chunk
        size_t n             /// number of requested tokens
        );


void dts_free( /// frees all resources
        dts_t* dts /// pointer to tokenstream
        );

#endif
