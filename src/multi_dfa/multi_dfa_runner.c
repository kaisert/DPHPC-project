#include<omp.h>
#include<parser.h>
#include<stdio.h>


/*
 * ./a.out <xml> <tokens> <dfa>
 */
int main(int argc, char* argv[]) {
    if(argc < 4) {
        perror("not enough parameters to main().");
    }
    
    multi_dfa_t multi_dfa;

    // init multi_dfa
    FILE* f_multi_dfa = fopen(argv[3], "r");
    init_multi_dfa(f_multi_dfa, &multi_dfa);
    fclose(f_multi_dfa);

    int n_threads = multi_dfa.no_dfa;

    // parse file
    Tokenstream* ts = parse_file(argv[1], argv[2], n_threads);

    // run dfas
    // start measurements

    int tid;
#pragma omp parallel num_threads(n_threads) firstprivate(map,chunks,num_chunks) private(tid)
    {
        tid = omp_get_thread_num() % n_threads;
        current_state = 
    }
    // stop measurements
    
    // output result
    
    // destroy tokenstream
    // tobias will write a function
}
