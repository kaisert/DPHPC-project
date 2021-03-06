#ifndef H_BUFSPLIT
#define H_BUFSPLIT

#include<stddef.h>

extern int bufsplit_split_xml_stream( // returns the number of chunks
        char* stream,     // < pointer to stream
        size_t len,       // < length of the stream
        size_t no_chunks, // < number of chunks
        char** offsets);  // < array with at least no_chunks+1 elements where
                          //   will be the offset of chunk i


#endif
