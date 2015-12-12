//
// Created by Stefan Dietiker on 07/12/15.
//

#ifndef DPHPC15_SUPERCHUNKTOKENIZER_H
#define DPHPC15_SUPERCHUNKTOKENIZER_H

#include <omp.h>
#include "../parser/parser.h"
#include "../chunker/test/Chunker.h"
#include<iterator>

class SuperChunkTokenizer {
public:
    template<typename tokenContainer, typename offsetContainer>
    void operator()(vector<tokenContainer> &ts, vector<offsetContainer> &os, Map* map, Chunker &chunker) {
        size_t no_chunks = 0, chunk_offset = 0, n_threads = omp_get_max_threads();
        no_chunks = chunker.no_chunks();
        while (chunk_offset < no_chunks) {
            n_threads = min(no_chunks - chunk_offset, n_threads);
#pragma omp parallel num_threads(n_threads) shared(map, chunker, ts)
            {
                int tid;
                char *chunk_begin, *chunk_end;


                tid = omp_get_thread_num() % n_threads;
                chunk_begin = chunker.get_chunk(chunk_offset + tid);
                chunk_end = chunker.get_chunk(chunk_offset + tid + 1);

                Parser parser(chunk_begin, chunk_end, map);
                
                tokenContainer& ts_container = ts.at(chunk_offset + tid);
                ts_container.init(map->size);

                auto backiter_ts = back_inserter(ts_container);
                auto backiter_off = back_inserter(os.at(chunk_offset + tid));
                parser.parse(backiter_ts, backiter_off);

                ts_container.flush();
            }

            chunk_offset += n_threads;
        }
    }
};


#endif //DPHPC15_SUPERCHUNKTOKENIZER_H
