#include<omp.h>
#include<parser.h>
#include<stdio.h>

#define STACK_SIZE 256


/*
 * ./a.out <xml> <tokens> <dfa> <output_file>
 */
int main(int argc, char* argv[]) {
    if(argc < 5) {
        perror("not enough parameters to main().");
        exit(1);
    }
    
    multi_dfa_t multi_dfa;

    // init multi_dfa
    FILE* f_multi_dfa = fopen(argv[3], "r");
    if(!f_multi_dfa) {
        perror("could not open multi_dfa file.");
        exit(1);
    }
    multidfa_init(f_multi_dfa, &multi_dfa);
    fclose(f_multi_dfa);

    int n_threads = multi_dfa.no_dfa;

    // read xml stream
    // chunk xml
    // tokenize -> tokenstream
    // parse file
    Tokenstream* ts = parse_file(argv[1], argv[2], n_threads);

    // run dfas
    // start measurements

#pragma omp parallel num_threads(n_threads) firstprivate(ts)
    {
        Tokenstream* ts_iter = ts;
        int tid = omp_get_thread_num() % n_threads;
        state_t q_cur = multi_dfa->start_states[tid];
        dfa_t dfa = multi_dfa->dfa_list[tid];

        // allocate stack
        int stack_pos = 0;
        state_t* dfa_stack = malloc(STACK_SIZE * sizeof(state_t));
        dfa_stack[stack_pos++] = q_cur;

        // iterate over tokenstream
        
        while(ts_iter != NULL) {
            Token* t = ts_iter->begin;

            while(t != ts_iter->end) {
                if(t->type < 0) {
                    if(stack_pos > 0)
                        stack_pos--;
                } else if(t->type > 0) {
                    dfa_stack[stack_pos+1] = multidfa_delta(
                            dfa, dfa_stack[stack_pos], t->type);
                    stack_pos++;
                }
                t++;
            }
            ts_iter = ts_iter->next;
        }
    }
    // stop measurements
    
    // output result
    FILE* f_output = fopen(argv[4], "w");
    if(!f_output) {
        perror("could not open output file.");
    }
    
    Tokenstream* ts_iter = ts;
    while(ts_iter != NULL) {
        Token* t = ts_iter->begin;
        while(t != ts_iter->end) {
            fprintf(f_output, "%ld, %ld\n", t->begin, t->end);
            t++;
        }

        ts_iter = ts_iter->next;
    }

    fclose(f_output);
    // destroy tokenstream
    destroy_tokenstream(ts);
    // tobias will write a function
}
