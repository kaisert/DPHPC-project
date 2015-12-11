//
// Created by Stefan Dietiker on 29/11/15.
//

#include<iostream>
#include "MultiDFA.h"

using namespace std;

MultiDFA::MultiDFA(string &fname) {
    ifstream is_dfas(fname);
    int no_dfas, alphabet_size, dfa_size, q_start, q;

    is_dfas >> no_dfas >> alphabet_size;

    for(int i = 0; i < no_dfas; ++i) {
        is_dfas >> dfa_size >> q_start;

        DFA* dfa = new DFA(q_start, dfa_size, alphabet_size);
        _dfas.push_back(dfa);
    }

    for(int i = 0; i < no_dfas; ++i) {
        DFA *dfa = _dfas.at(i);
        for (int j = 0; j < dfa->_state_table.size(); ++j) {
            is_dfas >> q;
            dfa->_state_table[j] = static_cast<state_t>(q);
        }
    }
}

MultiDFA::DFA::DFA(state_t start_state, size_t dfa_size, size_t alphabet_size) :
        _start_state(start_state), _alphabet_size(alphabet_size),
        _state_table(vector<state_t>(dfa_size*alphabet_size)) {}

MultiDFA::state_t MultiDFA::DFA::operator()(MultiDFA::state_t q, token_type_t t) {
    if(q < 0) { // accepting states are encoded as negative values
        q = -q - 1;
    }
    if(t > 0) {
        t--;
    }
    return _state_table[q*static_cast<int>(_alphabet_size) + t];
}

MultiDFA::~MultiDFA() {
    for(auto dfa_iter = _dfas.begin(); dfa_iter != _dfas.end(); ++dfa_iter) {
        delete *dfa_iter;
    }
}
