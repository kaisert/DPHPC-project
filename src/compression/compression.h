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

void* compression_init();

size_t compression_deflate( /// returns the actual number of bytes read
        void*,              /// < point to init struct
        char* src,          /// < source
        size_t n_src,       /// < chunk size
        char* dst,          /// < dest
        size_t dst_size
        );


size_t compression_inflate(  /// returns the actual number of bytes written
        void*,               /// < point to init struct
        char* src,           /// < source
        size_t n_src,        /// < chunk size
        char* dst,           /// < dest
        size_t dst_size
        );

void* compression_free(void*);

#endif
