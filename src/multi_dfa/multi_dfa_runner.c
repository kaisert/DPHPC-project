#include<omp.h>
#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/mman.h>

#include<token_list.h>
#include<tokenstream.h>
#include<parser.h>
#include<bufsplit.h>

#include"multi_dfa.h"

#define STACK_SIZE 256

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

    multi_dfa_t multi_dfa;
    int fd_xml, n_threads, no_chunks = 0;
    struct stat f_xml_stat;
    off_t xml_len;

    // init multi_dfa
    FILE* f_multi_dfa = fopen(ARG_DFA, "r");
    if(!f_multi_dfa) panic("could not open multi_dfa file.");
    multi_dfa_init(f_multi_dfa, &multi_dfa);
    fclose(f_multi_dfa);

    fd_xml = open(ARG_XML, O_RDWR);
    if(fd_xml < 0) panic("could not open xml file.");
    fstat(fd_xml, &f_xml_stat);
    xml_len = (off_t) f_xml_stat.st_size;

    char* xml_buf =
    mmap(   NULL,                  // address
            xml_len,               // length
            PROT_READ,             // prot flags
            MAP_FILE | MAP_SHARED, // flags
            fd_xml,                // file decriptor
            0);                    // offset in file
    if(xml_buf == MAP_FAILED) panic("mmap failed: ");
    

    n_threads = multi_dfa.no_dfa;

    // chunk xml stream
    char* chunks[n_threads+1];
    no_chunks = bufsplit_split_xml_stream(xml_buf, xml_len, n_threads, chunks);

    // printf("%d\n", no_chunks);
    // we want at least as many (meaningful) chunks as threads
    if(no_chunks != n_threads) panic("your xml is too small!");
   
    // initialize token stream
	Tokenstream* token_stream[n_threads];
    token_stream[0] = tokenstream_init((4096) / sizeof(Token));

	for(int i = 1; i < n_threads; ++i)
	{
        token_stream[i] = tokenstream_append(token_stream[i-1]);
	}

    // tokenize
	Map * map = alloc_map(ARG_TOKENS);
#pragma omp parallel num_threads(n_threads) firstprivate(map, chunks, token_stream)
	{
        int tid;
		char * chunk_begin, * chunk_end;
        Parser * parser;

		tid = omp_get_thread_num() % n_threads;
		chunk_begin = chunks[tid];
		chunk_end = chunks[tid + 1];

        printf("tid: %d\n", tid);
		parser = alloc_parser(chunk_begin, chunk_end);
		init_parser(parser, map);
		Tokenstream * ts_iter = token_stream[tid]; 
		Token * current = get_new_token_pointer(&ts_iter);
		while(get_next_token(parser, current) == 1)
		{
			current = get_new_token_pointer(&ts_iter);
		}
        tokenstream_rewind(ts_iter);
		free(parser);
	}
	
    destroy_map(map);

    // run dfas
    // start measurements

#pragma omp parallel num_threads(n_threads) firstprivate(token_stream, multi_dfa)
    {
        Tokenstream* ts_iter = token_stream[0];
        int tid = omp_get_thread_num() % n_threads;
        state_t q_cur = multi_dfa.start_states[tid];
        dfa_t dfa = multi_dfa.dfa_list[tid];

        // allocate stack
        int stack_pos = 0;
        state_t dfa_stack[STACK_SIZE];
        dfa_stack[stack_pos] = q_cur;

        // iterate over tokenstream
        while(ts_iter != NULL) {
            Token* t = ts_iter->begin;

            
            while(t != ts_iter->end) {
                if(t - ts_iter->begin < 16) {
#ifdef DEBUG
                    printf("token type %d:, pos: %ld, dfa state: %d"
                            "(pos: \"%.16s...\")\n",
                            t->type, t->begin - xml_buf, dfa_stack[stack_pos],
                            t->begin);
#endif
                }
                if(t->type < 0) {
                    if(stack_pos > 0) {
                        if(((int) dfa_stack[stack_pos]) < 0) {
                            printf("closing match: %.16s (pos: %ld)\n",
                                    t->begin, t->begin - xml_buf);
                        }
                        stack_pos--;
                    }
                } else if(t->type > 0) {
                    dfa_stack[stack_pos+1] = multi_dfa_delta(
                            dfa, dfa_stack[stack_pos], t->type);
                    stack_pos++;
                    if(dfa_stack[stack_pos] < 0) {
                            printf("open match: %.16s (pos: %ld)\n",
                                    t->begin, t->begin - xml_buf);
                    }
                }
                t++;
            }
            ts_iter = ts_iter->next;
        }
    }
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
    // fclose(f_output);
    // destroy tokenstream
    multi_dfa_free(&multi_dfa);
    destroy_tokenstreams(token_stream[0]);
    // tobias will write a function
}
