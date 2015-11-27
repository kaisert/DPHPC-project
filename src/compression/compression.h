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

#include<steddef.h>

#ifdef COMPR_Z_LIB
    #include<zlib.h>
#endif


#ifdef COMPR_Z_LIB
    #define COMPR_FLUSH Z_FLUSH
    #define COMPR_NO_FLUSH Z_NO_FLUSH
#elif COMPR_BITMASK
    // TODO
#endif

// TYPES
typedef struct compression_stream_struct {
    // General API
    size_t in_available;
    char* in;
    size_t out_available;
    char* out;
    // Specific to implementation
#ifdef COMPR_Z_LIB
    z_stream zstream;
#elif COMPR_BITMASK
    // TODO
#endif
} compression_stream_t;


typedef void* (*compression_init_func_t) (compression_stream_t*);
typedef int (*compression_func_t) (compression_stream_t*, bool);
typedef void (*compression_free_func_t) (void*, bool) ;


extern void* compression_inflate_init( /// returns handle for compression
        compression_stream_t* compression_stream
        );


extern void* compression_deflate_init( /// returns handle for compression
        compression_stream_t* compression_stream
        );


extern int compression_deflate( /// returns the actual number of bytes read
        compression_stream_t* compr_stream,
        int flags
        );


extern int compression_inflate(  /// returns the actual number of bytes written
        compression_stream_t* compr_stream,
        int flags
        );


extern void compression_free(
        void* handle,
        bool inflate
        );

#endif
