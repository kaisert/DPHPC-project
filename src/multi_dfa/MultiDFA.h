//
// Created by Stefan Dietiker on 29/11/15.
//

#ifndef DPHPC15_MULTIDFA_H
#define DPHPC15_MULTIDFA_H

#include<fstream>
#include <vector>

#include "../token_type.h"


using namespace std;

class MultiDFA {
public:
    typedef int16_t state_t;

    class DFA {
        friend class MultiDFA;
    public:
        DFA(state_t start_state, size_t dfa_size, size_t alphabet_size);
        state_t operator() (state_t q, token_type_t t);
        bool is_accepting(state_t q) { return q<0; }
        state_t start_state() { return _start_state; };
    private:
        size_t _alphabet_size;
        state_t _start_state;
        vector<state_t> _state_table;
    };

    MultiDFA(string& fname);

    ~MultiDFA();

    DFA* get_dfa(int i) {
        return _dfas.at(i);
    }

    size_t size() {
        return _dfas.size();
    }
private:
    vector<DFA*> _dfas;
};


#endif //DPHPC15_MULTIDFA_H
