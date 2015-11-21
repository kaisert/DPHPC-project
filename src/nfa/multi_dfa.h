#ifndef H_AUTOMATON
#define H_AUTOMATON

#include<stdio.h>
#include<stdint.h>

typedef int16_t token_t;
typedef int16_t state_t;
typedef token_t* dfa_t;

typedef struct {
    dfa_t* dfa_list;
    int no_dfa;
    int alphabet_size;
} multi_dfa_t;

int init_multi_dfa(FILE* f, multi_dfa_t* multi_dfa);

state_t delta(dfa_t dfa, state_t state, token_t t);

void free_multi_dfa(multi_dfa_t* multi_dfa);

#endif
