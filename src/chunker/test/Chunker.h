//
// Created by Stefan Dietiker on 03/12/15.
//

#include <vector>

#ifndef DPHPC15_CHUNKER_H
#define DPHPC15_CHUNKER_H

using namespace std;

#include <stddef.h>

class Chunker {
public:
    Chunker(char* buffer, size_t len, size_t min_chunk_len):
            _buffer(buffer), _buffer_len(len), _min_chunk_len(min_chunk_len) {
        int chunk_count = 1;
        char* cur = buffer, *cur_chunk = buffer, *end = buffer + len;

        _chunks.push_back(buffer);
        while(cur != end) {
            if(*cur == '<') {
                if(((size_t) (cur - cur_chunk)) >= min_chunk_len) {
                    _chunks.push_back(cur);
                    cur_chunk = cur;
                }
            }
            cur++;
        }
        _chunks.push_back(end);
    }

    char* get_chunk(int i) {
        return _chunks.at(i);
    }

    size_t no_chunks() {
        return _chunks.size()-1;
    }
private:
    char* _buffer;
    vector<char*> _chunks;
    size_t _min_chunk_len;
    size_t _buffer_len;
};


#endif //DPHPC15_CHUNKER_H
