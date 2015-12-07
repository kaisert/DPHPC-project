//
// Created by Stefan Dietiker on 07/12/15.
//

#ifndef DPHPC15_MATCH_H
#define DPHPC15_MATCH_H

#include "../parser/token_list.h"

struct Match {
    token_type_t token_type;
    unsigned long chunk_index;
    unsigned long token_index;
};

#endif //DPHPC15_MATCH_H
