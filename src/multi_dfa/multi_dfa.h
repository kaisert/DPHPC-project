#ifndef H_AUTOMATON
#define H_AUTOMATON

#include<stdio.h>
#include<stdint.h>
#include<token_list.h>

typedef int16_t state_t;
typedef token_type_t* dfa_t;

typedef struct {
    dfa_t* dfa_list;
    state_t* start_states;
    int no_dfa;
    int alphabet_size;
} multi_dfa_t;

extern int multi_dfa_init(FILE* f, multi_dfa_t* multi_dfa);

extern state_t multi_dfa_delta(dfa_t dfa, state_t state, token_type_t t);

extern void multi_dfa_free(multi_dfa_t* multi_dfa);

#endif
