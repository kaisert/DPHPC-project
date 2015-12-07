//
// Created by Stefan Dietiker on 07/12/15.
//

#ifndef DPHPC15_MATCHER_H
#define DPHPC15_MATCHER_H

#include<vector>
#include <stdint-gcc.h>
#include "../parser/token_list.h"
#include "../tokenizer/Tokenizer.h"
#include "../multi_dfa/MultiDFA.h"

using namespace std;

struct Match {
    token_type_t token_type;
    unsigned long chunk_index;
    unsigned long token_index;
};

using matchstream_t=vector<vector<Match> >;

class Matcher {
public:
    virtual void operator() (matchstream_t &ms, tokenstream_t &ts, MultiDFA& multiDFA) = 0;
};

#endif //DPHPC15_MATCHER_H
