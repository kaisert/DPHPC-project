//
// Created by Stefan Dietiker on 07/12/15.
//

#ifndef DPHPC15_DFAMATCHER_H
#define DPHPC15_DFAMATCHER_H


#include "Matcher.h"
#include "../multi_dfa/MultiDFA.h"

#define STACK_SIZE 256

class DFAMatcher: public Matcher {

public:
    virtual void operator()(matchstream_t &ms, tokenstream_t &ts, MultiDFA& multiDFA) override;
};


#endif //DPHPC15_DFAMATCHER_H
