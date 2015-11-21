#include"multi_dfa.h"
#include<stdio.h>
#include<stdlib.h>

int fscan_int(FILE* f, int* p) {
    return fscanf(f, "%i\n", p);
}

int __multi_dfa_alphabet_size;
token_t *__multi_dfa_state_table;

/*
 * init_multi_dfa
 *
 * initializes a multi DFA from a file containing one text integer per line.
 *
 * THIS FUNCTION IS NOT THREAD-SAFE.
 */
int init_multi_dfa(FILE* f, multi_dfa_t* multi_dfa) {
    int i, dfa_size, table_size = 0;

    fscan_int(f, &(multi_dfa->no_dfa));
    fscan_int(f, &__multi_dfa_alphabet_size);

    printf("dfas %i\n", multi_dfa->no_dfa);
    multi_dfa->alphabet_size = __multi_dfa_alphabet_size;
    
    int dfa_offsets[multi_dfa->no_dfa];

    for( i = 0; i < multi_dfa->no_dfa; ++i ) {
        dfa_offsets[i] = table_size;
        fscan_int(f, &dfa_size);
        table_size += dfa_size * multi_dfa->alphabet_size;
    }

    __multi_dfa_state_table = (state_t*) malloc(table_size * sizeof(state_t));

    for( i = 0; i < table_size; ++i ) {
        int _q;
        fscan_int(f, &_q);
        __multi_dfa_state_table[i] = _q;
    }

    multi_dfa->dfa_list = (dfa_t *) malloc(multi_dfa->no_dfa * sizeof(dfa_t));

    for( i = 0; i < multi_dfa->no_dfa; ++i ) {
        multi_dfa->dfa_list[i] = &__multi_dfa_state_table[0] + dfa_offsets[i];
    }

    return multi_dfa->no_dfa;
}


/*
 * delta
 *
 * the transition function for a dfa
 */
state_t delta(dfa_t dfa, state_t state, token_t t) {
    if(state < 0) { // accepting states are encoded as negative values
        state = -state - 1;
    }
    return *(dfa + state*__multi_dfa_alphabet_size + t);
}

/*
 * free_multi_dfa
 */
void free_multi_dfa(multi_dfa_t* multi_dfa) {
    free(__multi_dfa_state_table);
    free(multi_dfa->dfa_list);
}
