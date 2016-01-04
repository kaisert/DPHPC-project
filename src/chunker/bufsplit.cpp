#include<stddef.h>

int bufsplit_split_xml_stream(
        char* stream,
        size_t len,
        size_t no_chunks,
        char** offsets) {

    int chunk_count = 1;
    char* cur = stream, *cur_chunk = stream, *end = stream + len;
    size_t min_chunk_len = len/no_chunks;
    
    offsets[0] = stream;
#ifndef __APPLE__
    //hack for relative comparison of pointers
    while((unsigned long long) cur < (unsigned long long) end) {
#else
    while(cur != end) {
#endif
        if(*cur == '<') {
            if(((size_t) (cur - cur_chunk)) >= min_chunk_len) {
                offsets[chunk_count++] = cur;
                cur_chunk = cur;
#ifndef __APPLE__
                cur += min_chunk_len;
#endif
            }
        }
        cur++;
    }
    offsets[chunk_count] = end;
    return chunk_count;
}
