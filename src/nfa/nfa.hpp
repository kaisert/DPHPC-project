#ifndef NFA_H
#define NFA_H

#include<cstdint>
#include<vector>

typedef uint32_t nfa_token_t;
typedef uint32_t nfa_state_t;

bool is_closing_token(nfa_token_t) {
    return nfa_token_t < 0;
}

using namespace std;

typedef vector<nfa_state_t>::const_iterator nfa_state_iterator;
typedef pair<nfa_state_iterator, nfa_state_iterator> nfa_state_set;

namespace Nfa {
    class Delta {
        public:
            nfa_state_t no_states;
            nfa_token_t no_tokens;
            nfa_state_t start_state;

            Delta(string fname) {
                vector<uint32_t> offset_table;
                vector<nfa_state_t> state_table;
                map<uint32_t, nfa_state_set> offset_mapping;

                ifstream f_delta_table(fname, ifstream::binary);
                f_delta_table >> no_states
                    >> no_tokens
                    >> start_state;

                // building the offset table
                uint32_t offset;
                for(nfa_state_t q = 0; q < no_states; q++) {
                    for(nfa_token_t t = 0; t < no_tokens; t++) {
                        f_delta_table >> offset;
                        offset_table.push_back(offset);
                    }
                }

                // read state sets
                uint32_t set_len, offset_start, offset = 0;
                while(f_delta_table >> set_len) {
                    offset_start = offset;
                    nfa_state_t q;
                    for(uint32_t i=0; i < set_len; i++) {
                        f_delta_table >> q;
                        state_table.push_back(q);
                        offset++;
                    }
                    offset_mapping.push_back(make_pair(
                                state_table.begin()+offset_start,
                                state_table.begin()+offset));
                }

                for(nfa_state_t q = 0; q < no_states; q++) {
                    for(nfa_token_t t = 0; t < no_tokens; t++) {
                        state_set_mapping.push_back(
                                offset_mapping[offset_table.at(q * no_tokens +
                                    t)]);
                    }
                }
            }

            nfa_state_set operator() (nfa_state_t q,
                    nfa_token_t tok) {
                return state_set_mapping[q * no_tokens + t];
            }

        private:
            vector<nfa_state_set> state_set_mapping;
    };

    class Nfa {
        public:
            Delta delta_fwd;
            Delta delta_bk;

            Nfa(string fname) {
                delta_fwd = Delta(fname + ".dfwd");
                delta_bk = Delta(fname + ".dbk");
            }
    };

}
