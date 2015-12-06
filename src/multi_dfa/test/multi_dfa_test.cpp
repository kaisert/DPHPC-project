#include<stdio.h>
#include<multi_dfa.h>

const int EXIT_SUCCESS = 0;
const int EXIT_FAILURE = 1;

int main(int argc, char* argv[]) {
    if(argc < 2)
        return EXIT_FAILURE;

    FILE* f = fopen(argv[1], "r");

    if(f) {
        multi_dfa_t m_dfa;

        int no_dfa = init_multi_dfa(f, &m_dfa);

        printf("Number of read dfas: %i\n", no_dfa);
        fclose(f);


        free_multi_dfa(&m_dfa);
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}
