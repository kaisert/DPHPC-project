#include<iostream>
#include"nfa.hpp"

using namespace std;

int main(int argc, char* argv[]) { 
    if(argc > 1) {
        cout << argv[0] << endl;
        string fname(argv[1]);
        Nfa::Nfa nfa(fname);
        
        Nfa::nfa_token_t t;
        Nfa::nfa_state_t q;

        for(q = 0; q < nfa.no_states; q++) {
            for(t = 0; t < nfa.no_tokens; t++) {
                cout << "Transitions for " << q << " " << t << endl;
                Nfa::nfa_state_set state_set = nfa.delta_fwd(q, t);
                for(auto iter = state_set.first; iter != state_set.second;
                        iter++) {
                    cout << *iter << endl;
                }
            }
        }
    }
}
