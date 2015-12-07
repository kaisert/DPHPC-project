//
// Created by Stefan Dietiker on 07/12/15.
//

#ifndef DPHPC15_TOKENIZER_H
#define DPHPC15_TOKENIZER_H

#include "../parser/token_list.h"
#include "../chunker/test/Chunker.h"
#include "../parser/map.h"
#include<vector>

using namespace std;

using tokenstream_t=vector<vector<token_type_t> >;
using offsetstream_t=vector<vector<char*> >;

class Tokenizer {
public:
    virtual ~Tokenizer() {};
    virtual void operator() (tokenstream_t &ts, offsetstream_t &os, Map* map, Chunker &chunker) = 0;
};

#endif //DPHPC15_TOKENIZER_H
