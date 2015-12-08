//
// Created by Stefan Dietiker on 07/12/15.
//

#ifndef DPHPC15_DFAMATCHER_H
#define DPHPC15_DFAMATCHER_H

#include "../multi_dfa/MultiDFA.h"
#include "Match.h"

#define STACK_SIZE 256

class DFAMatcher {

public:
    template<typename MatchContainerType, typename TokenContainerType>
    void operator() (vector<MatchContainerType> &ms, vector<TokenContainerType> &ts, MultiDFA& multiDFA) {
        int n_threads = multiDFA.size();
#pragma omp parallel num_threads(n_threads) shared(multiDFA, ms, ts)
        {
            int tid = omp_get_thread_num() % n_threads;
            MultiDFA::DFA* dfa = multiDFA.get_dfa(tid);
            MultiDFA::state_t q_cur = dfa->start_state();

            auto& my_matches = ms.at(tid);

            // allocate/initialize stack
            int stack_pos = 0;
            MultiDFA::state_t dfa_stack[STACK_SIZE];
            dfa_stack[stack_pos] = q_cur;

            for(uint32_t i = 0; i != ts.size(); ++i) {
                auto &cur_stream = ts[i];
                uint32_t j = 0;
                //for (uint32_t j = 0; j != cur_stream.size(); ++j) {
                //    token_type_t cur_token = cur_stream[j];
                for(auto t_it = cur_stream.begin(); t_it != cur_stream.end(); t_it++, ++j) {
                    token_type_t cur_token = *t_it;
                    if (cur_token < 0) {
                        if (stack_pos > 0) {
                            if (((int) dfa_stack[stack_pos]) < 0) {
                                my_matches.push_back(Match {cur_token, i, j});
                            }
                            stack_pos--;
                        }
                    } else if (cur_token > 0) {
                        dfa_stack[stack_pos + 1] = (*dfa)(dfa_stack[stack_pos], cur_token);
                        stack_pos++;
                        if (dfa_stack[stack_pos] < 0) {
                            my_matches.push_back(Match {cur_token, i, j});
                        }
                    }
                }
            }
            //matches[tid] = my_matches;
        } // matcher
    }
};


#endif //DPHPC15_DFAMATCHER_H
