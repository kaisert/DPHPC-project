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
    uint8_t* in;
    size_t out_available;
    uint8_t* out;
    // Specific to implementation
    void* stream_state;
} compression_stream_t;


typedef int (*compression_init_func_t) (compression_stream_t*);
typedef int (*compression_func_t) (compression_stream_t*, int);
typedef void (*compression_end_func_t) (compression_stream_t*) ;

typedef struct compression_api_struct {
    compression_init_func_t compression_deflate_init;
    compression_init_func_t compression_inflate_init;
    compression_func_t compression_deflate;
    compression_func_t compression_inflate;
    compression_end_func_t compression_end_deflate;
    compression_end_func_t compression_end_inflate;
    unsigned long reserved;
} compression_api_t;

/*
 * The compression_api_t is an abstract type which acts as an interface to
 * conrete compression implementations. Hence, if you want to create a module
 * which provides zlib compression function, you would create a function called
 *
 * extern void compression_zlib_api(compression_api_t *compression_api);
 *
 * It is completely up to the provider of a concrete compression module how the
 * abstract compression API is initialized.
 *
 * Example: Let the above function be the API construction for a zlib-module,
 * then a user would initialize an deflate stream as follows:
 *
 * compression_api_t compression_api;
 * compression_stream_t compression_stream;
 *
 * compression_zlib_api(&compr_api);
 *
 * compr_api.compression_deflate_init(&compression_stream);
 */

#endif
