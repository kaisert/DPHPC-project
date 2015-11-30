#ifdef _OPENMP
#include<omp.h>
#else
#define omp_get_thread_num() 0
#endif

#include<iostream>
#include<fstream>

#include<fcntl.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/mman.h>

#include"parser/token_list.h"
#include"parser/parser.h"
#include"chunker/bufsplit.h"

#include"multi_dfa/MultiDFA.h"
#include "timing/GlobalTicToc.h"

#define STACK_SIZE 256

using namespace std;

struct Match {
    token_type_t token_type;
    uint32_t chunk_index;
    uint32_t token_index;
};

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

    GlobalTicToc globalTicToc;

    // init mdfa
    string fname_dfas = string(ARG_DFA);
    MultiDFA multiDFA(fname_dfas);

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
    globalTicToc.start_phase();
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
    globalTicToc.stop_phase("tokenizer");
    destroy_map(map);

    vector<vector<Match> > matches(n_threads);
    // run dfas
    // start measurements
    globalTicToc.start_phase();
#pragma omp parallel num_threads(n_threads) firstprivate(token_stream, multi_dfa)
    {
        int tid = omp_get_thread_num() % n_threads;
        MultiDFA::DFA* dfa = multiDFA.get_dfa(tid);
        MultiDFA::state_t q_cur = dfa->start_state();

        auto my_matches = matches.at(tid);

        // allocate/initialize stack
        int stack_pos = 0;
        MultiDFA::state_t dfa_stack[STACK_SIZE];
        dfa_stack[stack_pos] = q_cur;

        for(uint32_t i = 0; i != token_streams.size(); ++i) {
            vector<token_type_t> cur_stream = token_streams[i];
            for (uint32_t j = 0; j != cur_stream.size(); ++j) {
                token_type_t cur_token = cur_stream[j];
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
        matches[tid] = my_matches;
    }
    globalTicToc.stop_phase("matcher");

    ofstream of_results(ARG_OUTPUT);

    for(uint32_t query = 0; query != matches.size(); ++query) {
        for(auto match = matches[query].begin(); match != matches[query].end(); ++match) {
            char* buf_offset = offset_streams[match->chunk_index][match->token_index];
            uint64_t file_offset = reinterpret_cast<uint64_t>(buf_offset)
                    - reinterpret_cast<uint64_t>(xml_buf);
            of_results << query << ", " << file_offset << ", " << (match->token_type < 0 ? "c" : "o") << endl;
        }
    }

    cout << "Timing summary:" << endl;
    for(auto phase = globalTicToc.begin(); phase != globalTicToc.end(); ++phase) {
        cout << phase->first << ": " << globalTicToc.get_phase_period<chrono::milliseconds>(phase->first) <<
                " ms" << endl;
    }
    cout << "Total Time: " << globalTicToc.get_total_time<chrono::milliseconds>() << " ms" << endl;

    close(fd_xml);
    munmap(xml_buf, xml_len);
}
