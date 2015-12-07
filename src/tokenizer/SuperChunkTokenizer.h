//
// Created by Stefan Dietiker on 07/12/15.
//

#ifndef DPHPC15_SUPERCHUNKTOKENIZER_H
#define DPHPC15_SUPERCHUNKTOKENIZER_H

#include "Tokenizer.h"

class SuperChunkTokenizer: public Tokenizer {
public:
    virtual void operator()(tokenstream_t &ts, offsetstream_t &os, Map* map, Chunker &chunker) override;
};


#endif //DPHPC15_SUPERCHUNKTOKENIZER_H
