#ifdef _OPENMP
#include<omp.h>
#else
#define omp_get_thread_num() 0
#endif

#include<iostream>
#include<fstream>

#include<unistd.h>
#include <thread>

#include"parser/token_list.h"
#include"parser/parser.h"

#include"multi_dfa/MultiDFA.h"
#include "timing/GlobalTicToc.h"
#include "chunker/test/Chunker.h"
#include "exceptions/GeneralException.h"
#include"config_local.h"

using namespace std;

#define panic(msg) \
  do { perror(msg); exit(EXIT_FAILURE); } while (0)

/*
 * ./a.out <xml> <tokens> <dfa> <output_file>
 */
int main(int argc, char* argv[]) {
    if(argc < 4) panic("not enough parameters to main(): \
<XML> <TOKENS> <MDFA> [<OUTPUT_LOGFILE>]");
#define ARG_XML argv[1]
#define ARG_TOKENS argv[2]
#define ARG_DFA argv[3]
#define ARG_OUTPUT argv[4]
    size_t xml_len, no_chunks;
    int n_threads;
    char* xml_buf;

    GlobalTicToc globalTicToc;

    globalTicToc.start_phase();
    usleep(1000000);
    globalTicToc.stop_phase("00. calibration ;-)");

    // init mdfa
    string fname_dfas = string(ARG_DFA);
    MultiDFA multiDFA(fname_dfas);

    shared_ptr<Loader> loader(config::loader);
    // ############ LAODING
    try {
        globalTicToc.start_phase();
        xml_buf = (*loader)(ARG_XML);
        xml_len = loader->length();
        globalTicToc.stop_phase("01. loading");
    } catch(GeneralException& generalException) {
        panic(generalException.what());
    }
    // ############

    n_threads = omp_get_max_threads();

    cout << "#threads: " << n_threads << endl;

    // ############ CHUNKING
    globalTicToc.start_phase();
    // chunk xml stream
    size_t chunk_size = min( (xml_len / (static_cast<size_t>(n_threads))), static_cast<size_t>(DEFAULT_CHUNK_SIZE));
    Chunker chunker(xml_buf, xml_len, chunk_size);
    no_chunks = chunker.no_chunks();
    globalTicToc.stop_phase("02. chunking");
    // ############

    // printf("%d\n", no_chunks);
    // we want at least as many (meaningful) chunks as threads
    if(no_chunks < n_threads) panic("your xml is too small!");

    // initialize token stream
    tokenstream_t token_streams(no_chunks);
    for(auto ts_iter = token_streams.begin(); ts_iter != token_streams.end();
            ++ts_iter) {
        ts_iter->reserve(STREAM_RESERVE_MEMORY);
    }

    // initialize offset stream
    offsetstream_t offset_streams(no_chunks);
    for(auto of_iter = offset_streams.begin(); of_iter != offset_streams.end();
            ++of_iter) {
        of_iter->reserve(STREAM_RESERVE_MEMORY);
    }

    shared_ptr<Tokenizer> tokenizer(config::tokenizer);
    // ############ TOKENIZE
    Map * map = alloc_map(ARG_TOKENS);
    globalTicToc.start_phase();
    (*tokenizer)(token_streams, offset_streams, map, chunker);
    globalTicToc.stop_phase("03. tokenizer");
    destroy_map(map);
    // ############ END OF TOKENIZE


    n_threads = multiDFA.size();
    // initialize memory for matches
    matchstream_t matches(n_threads);
    for(auto it_match = matches.begin(); it_match != matches.end(); ++it_match) {
        it_match->reserve(STREAM_RESERVE_MEMORY);
    }

    // count #tokens
    uint64_t no_tokens = 0;
    for(auto ts_iter = token_streams.begin(); ts_iter != token_streams.end(); ++ts_iter) {
        no_tokens += ts_iter->size();
    }

    cout << "#tokens: " << no_tokens << " (total)" << endl;

    shared_ptr<Matcher> matcher(config::matcher);
    // run dfas
    // start measurements
    globalTicToc.start_phase();
    (*matcher)(matches, token_streams, multiDFA);
    globalTicToc.stop_phase("04. matcher");

    globalTicToc.start_phase();
    if(argc > 4) {
        ofstream of_results(ARG_OUTPUT);

        for (uint32_t query = 0; query != matches.size(); ++query) {
            for (auto match = matches[query].begin(); match != matches[query].end(); ++match) {
                char *buf_offset = offset_streams[match->chunk_index][match->token_index];
                uint64_t file_offset = reinterpret_cast<uint64_t>(buf_offset)
                                       - reinterpret_cast<uint64_t>(xml_buf);
                of_results << query << ", " << file_offset << ", " << (match->token_type < 0 ? "c" : "o") << endl;
            }
        }
    }
    globalTicToc.stop_phase("05. write back");

    cout << "XML-File: " << ARG_XML << endl;
    cout << "Timing summary:" << endl;
    for(auto phase = globalTicToc.begin(); phase != globalTicToc.end(); ++phase) {
        cout << phase->first << ": " << globalTicToc.get_phase_period<chrono::milliseconds>(phase->first) <<
                " ms" << endl;
    }
    cout << "Total Time: " << globalTicToc.get_total_time<chrono::milliseconds>() << " ms" << endl;

}
