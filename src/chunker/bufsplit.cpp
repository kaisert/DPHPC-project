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
    while(cur != end) {
        if(*cur == '<') {
            if(((size_t) (cur - cur_chunk)) >= min_chunk_len) {
                offsets[chunk_count++] = cur;
                cur_chunk = cur;
            }
        }
        cur++;     
    }
    offsets[chunk_count] = end;
    return chunk_count;
}
