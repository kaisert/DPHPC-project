/*
 * compression.h
 * *************
 *
 * API for compression. The compression operates on a byte level to be as
 * general as possible.
 *
 */

#ifndef H_COMPRESSION
#define H_COMPRESSION

#include<stddef.h>
#include<zlib.h>

// we use the same API as Z_LIB
#define COMPR_FLUSH Z_FLUSH
#define COMPR_NO_FLUSH Z_NO_FLUSH

// For small return values, we use the same values as Z_LIB
#define COMPR_INIT_ERROR -129

// TYPES
typedef struct compression_stream_struct {
    // General API
    size_t in_available;
    char* in;
    size_t out_available;
    char* out;
    // Specific to implementation
    void* stream_state;
} compression_stream_t;


typedef void* (*compression_init_func_t) (compression_stream_t*);
typedef int (*compression_func_t) (compression_stream_t*, bool);
typedef void (*compression_free_func_t) (void*, bool) ;


/*
 * All functions return code for state. Positive values indicate succes and
 * possibly and special state. Negative values indicate an error state.
 *
 * For _init()-funtions, the user is responsible for the memory management of
 * the structure that is passed to the function.
 */

extern int compression_inflate_init( /// initializes handle for inflation
        compression_stream_t* compression_stream
        );


extern int compression_deflate_init( /// initializes handle for deflation
        compression_stream_t* compression_stream
        );


extern int compression_deflate( /// returns error state
        compression_stream_t* compr_stream,
        int flags /// e.g., COMPR_FLUSH
        );


extern int compression_inflate(  /// returns error state
        compression_stream_t* compr_stream,
        int flags
        );


extern void compression_inflate_end( /// 
        compression_stream_t* compr_stream
        );

extern void compression_deflate_end( /// 
        compression_stream_t* compr_stream
        );
#endif
