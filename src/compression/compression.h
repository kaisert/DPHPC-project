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

typedef void* (*compression_init_func_t) (bool);
typedef size_t (*compression_func_t) (void*, char*, size_t, char*, size_t);
typedef void (*compression_free_func_t) (void*, bool) ;

extern void* compression_init( /// returns handle for compression
        bool inflate);         /// < indicates whether you which to inflate or deflate


extern size_t compression_deflate( /// returns the actual number of bytes read
        void*,                     /// < point to init struct
        char* src,                 /// < source
        size_t n_src,              /// < chunk size
        char* dst,                 /// < dest
        size_t dst_size
        );


extern size_t compression_inflate(  /// returns the actual number of bytes written
        void*,                      /// < point to init struct
        char* src,                  /// < source
        size_t n_src,               /// < chunk size
        char* dst,                  /// < dest
        size_t dst_size
        );


extern void compression_free(void* handle,
        bool inflate);

#endif
