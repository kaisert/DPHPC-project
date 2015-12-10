#ifndef H_BUFSPLIT
#define H_BUFSPLIT

#include<cstddef>

extern int bufsplit_split_xml_stream( // returns the number of chunks
        char* stream,     // < pointer to stream
        std::size_t len,       // < length of the stream
        std::size_t no_chunks, // < number of chunks
        char** offsets);  // < array with at least no_chunks+1 elements where
                          //   will be the offset of chunk i


#endif
