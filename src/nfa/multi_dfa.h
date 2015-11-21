#ifndef H_AUTOMATON
#define H_AUTOMATON

#include<stdint.h>

typedef int16_t token_t;
typedef uint16_t state_t;
typedef *token_t dfa_t;

typedef struct {
    dfa_t* dfa_list;
    int no_dfa;
    int alphabet_size;
} multi_dfa_t;

int init_multi_dfa(FILE* filename, multi_dfa_t* multi_dfa);

state_t delta(dfa_t dfa, state_t state, token_t);

void free_multi_dfa(multi_dfa_t* multi_dfa);

#endif
