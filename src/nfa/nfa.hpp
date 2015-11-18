#ifndef NFA_H
#define NFA_H

#include<iostream>
#include<fstream>
#include<cstdint>
#include<vector>
#include<map>
#include<tuple>
#include<unistd.h>

using namespace std;

namespace Nfa {
    typedef uint32_t nfa_token_t;
    typedef uint32_t nfa_state_t;
    typedef vector<nfa_state_t>::const_iterator nfa_state_iterator;
    typedef pair<nfa_state_iterator, nfa_state_iterator> nfa_state_set;

    bool is_closing_token(nfa_token_t t) {
        return (t&(1<<31)) != 0;
    }

    class Delta {
        public:
            nfa_state_t no_states;
            nfa_token_t no_tokens;
            nfa_state_t start_state;

            Delta(string fname) {
                vector<uint32_t> offset_table;
                vector<nfa_state_t> state_table;
                map<uint32_t, nfa_state_set >
                    offset_mapping;

                cout << "filename is " << fname << endl;

                ifstream f_delta_table(fname, ifstream::binary);
                __read(f_delta_table, &no_states);
                __read(f_delta_table, &no_tokens);
                __read(f_delta_table, &start_state);

                // building the offset table
                uint32_t offset;
                for(nfa_state_t q = 0; q < no_states; q++) {
                    for(nfa_token_t t = 0; t < no_tokens; t++) {
                        __read(f_delta_table, &offset);
                        cout << q << " " << t << ": " << offset << endl;
                        offset_table.push_back(offset);
                    }
                }
                
                long cur_pos = f_delta_table.tellg();
                f_delta_table.seekg(0, ifstream::end);
                long state_table_sz = ((long) f_delta_table.tellg()) - cur_pos;
                f_delta_table.seekg(cur_pos);

                state_table.reserve(state_table_sz);

                // read state sets
                uint32_t set_len, offset_start;
                offset = 0;
                uint32_t fstream_offset = 0;
                while(__read(f_delta_table, &set_len)) {
                    cout << fstream_offset << " " << set_len << " " << endl;
                    offset_start = offset;
                    nfa_state_t q;
                    for(uint32_t i=0; i < set_len; i++) {
                        __read(f_delta_table, &q);
                        cout << q;
                        state_table.push_back(q);
                        offset++;
                    }
                    cout << endl;
                    offset_mapping[fstream_offset] = make_pair(
                                state_table.begin()+offset_start,
                                state_table.begin()+offset);
                    fstream_offset += set_len + 1;
                }

                for(nfa_state_t q = 0; q < no_states; q++) {
                    for(nfa_token_t t = 0; t < no_tokens; t++) {
                        cout << "states " ;
                        for(auto iter = offset_mapping[offset_table.at(
                            q * no_tokens + t)].first;
                                iter != offset_mapping[
                                offset_table.at(q*no_tokens + t)].second;
                                iter++){
                            cout << " " << *iter << endl;
                        }
                        cout << endl;
                        state_set_mapping.push_back(
                                offset_mapping[offset_table.at(q * no_tokens +
                                    t)]);
                    }
                }
            }

            nfa_state_set operator() (nfa_state_t q,
                    nfa_token_t t) {
                return state_set_mapping[q * no_tokens + t];
            }

        private:
            vector<nfa_state_set> state_set_mapping;

            template<typename T>
            istream& __read(ifstream &f, T* r) {
                return f.read(reinterpret_cast<char*>(r), sizeof(T));
            }

    };

    class Nfa {
        public:
            Delta delta_fwd;
            Delta delta_bk;
            nfa_state_t no_states;
            nfa_state_t start_state;
            nfa_token_t no_tokens;

            Nfa(string fname): delta_fwd(Delta(fname + ".dfwd")),
                delta_bk(Delta(fname + ".dbk")) {
                    no_states = delta_fwd.no_states;
                    no_tokens = delta_fwd.no_tokens;
                    start_state = delta_fwd.start_state;
            }
    };

}

#endif
