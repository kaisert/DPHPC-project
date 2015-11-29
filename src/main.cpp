#ifdef _OPENMP
#include<omp.h>
#else
#define omp_get_thread_num() 0
#endif

#include<iostream>

#include<fcntl.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/mman.h>

#include"parser/token_list.h"
#include"parser/parser.h"
#include"chunker/bufsplit.h"

#include"multi_dfa/MultiDFA.h"

#define STACK_SIZE 256

using namespace std;

#define panic(msg) \
  do { perror(msg); exit(EXIT_FAILURE); } while (0)

/*
 * ./a.out <xml> <tokens> <dfa> <output_file>
 */
int main(int argc, char* argv[]) {
    if(argc < 5) panic("not enough parameters to main().");
#define ARG_XML argv[1]
#define ARG_TOKENS argv[2]
#define ARG_DFA argv[3]
#define ARG_OUTPUT argv[4]
    int fd_xml, n_threads, no_chunks = 0;
    struct stat f_xml_stat;
    off_t xml_len;

    // init mdfa
    string fname_dfas = string(ARG_DFA);
    MultiDFA multiDFA(fname_dfas);

    cout << "mapping file..." << endl;
    fd_xml = open(ARG_XML, O_RDWR);
    if(fd_xml < 0) panic("could not open xml file.");
    fstat(fd_xml, &f_xml_stat);
    xml_len = (off_t) f_xml_stat.st_size;



    char* xml_buf = static_cast<char*>(
            mmap(   NULL,                  // address
                    xml_len,               // length
                    PROT_READ,             // prot flags
                    MAP_FILE | MAP_SHARED, // flags
                    fd_xml,                // file decriptor
                    0)                     // offset in file
    );
    if(xml_buf == MAP_FAILED) panic("mmap failed: ");


    n_threads = multiDFA.size();

    // chunk xml stream
    char* chunks[n_threads+1];
    no_chunks = bufsplit_split_xml_stream(xml_buf, xml_len, n_threads, chunks);

    // printf("%d\n", no_chunks);
    // we want at least as many (meaningful) chunks as threads
    if(no_chunks != n_threads) panic("your xml is too small!");

    // initialize token stream
    vector<vector<token_type_t> > token_streams(n_threads);
    vector<vector<char*> > offset_streams(n_threads);

    // tokenize
    Map * map = alloc_map(ARG_TOKENS);
#pragma omp parallel num_threads(n_threads) firstprivate(map, chunks, token_stream)
    {
        int tid;
        char * chunk_begin, * chunk_end;


        tid = omp_get_thread_num() % n_threads;
        chunk_begin = chunks[tid];
        chunk_end = chunks[tid + 1];

        Parser parser(chunk_begin, chunk_end, map);

        auto backiter_ts = back_inserter(token_streams.at(tid));
        auto backiter_off = back_inserter(offset_streams.at(tid));
        parser.parse(backiter_ts, backiter_off);
    }

    destroy_map(map);

    vector<vector<uint64_t > > matching_offsets(n_threads);
    // run dfas
    // start measurements
#pragma omp parallel num_threads(n_threads) firstprivate(token_stream, multi_dfa)
    {
        int tid = omp_get_thread_num() % n_threads;
        MultiDFA::DFA* dfa = multiDFA.get_dfa(tid);
        MultiDFA::state_t q_cur = dfa->start_state();

        auto my_matches = matching_offsets.at(tid);

        // allocate stack
        uint64_t cur_match_off = 0;
        int stack_pos = 0;

        MultiDFA::state_t dfa_stack[STACK_SIZE];
        dfa_stack[stack_pos] = q_cur;

        for(auto ts_i = token_streams.begin(); ts_i != token_streams.end(); ++ts_i) {
            auto t_end = ts_i->end();
            for(auto t_i = ts_i->begin(); t_i != t_end; ++t_i) {
                if(*t_i < 0) {
                    if(stack_pos > 0) {
                        if(((int) dfa_stack[stack_pos]) < 0) {
                            my_matches.push_back(cur_match_off);
                        }
                        stack_pos--;
                    }
                } else if(*t_i > 0) {
                    dfa_stack[stack_pos+1] = (*dfa)(dfa_stack[stack_pos], *t_i);
                    stack_pos++;
                    if(dfa_stack[stack_pos] < 0) {
                        my_matches.push_back(cur_match_off);
                    }
                }
                cur_match_off++;
            }
        }
    }

    /*
    for(auto query = matching_offsets.begin(); query != matching_offsets.end();
        ++query) {
        for(auto match = query->begin(); match != query->end(); ++match) {
            cout << "match at" <<
                    reinterpret_cast<uint64_t >((*match - xml_buf)) << endl;
        }
    }*/

    // stop measurements

    // output result
    /*
    FILE* f_output = fopen(argv[4], "w");
    if(!f_output) {
        perror("could not open output file.");
    }

    Tokenstream* ts_iter = token_stream[0];
    while(ts_iter != NULL) {
        Token* t = ts_iter->begin;
        while(t != ts_iter->end) {
            fprintf(f_output, "%ld, %ld\n", t->begin, t->end);
            t++;
        }

        ts_iter = ts_iter->next;
    }
    */

    close(fd_xml);
    munmap(xml_buf, xml_len);
    // fclose(f_output);
    // destroy tokenstream
    // tobias will write a function
}
