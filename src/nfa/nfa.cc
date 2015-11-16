#ifndef NFA_H
#define NFA_H

#include<cstdint>

typedef int32_t nfa_token_t;
typedef int32_t nfa_state_t;

bool is_closing_token(nfa_token_t) {
    return nfa_token_t < 0;
}

class Nfa {
    public:
        // for now, the delta functions return vectors. It might be more
        // efficient to pass a std::back_inserter as an argument and let the
        // implementation directly insert the resulting states

        virtual void delta_push(t_nfa_state, t_token tok)
            = 0;

        // delta_unknown returns all states which 
        virtual std::vector<nfa_state_t> delta_unkown(t_nfa_state, t_token tok)
            = 0;
};
