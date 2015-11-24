#include<omp.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/mman.h>

#include<parser.h>
#include<bufsplit.h>

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
    int fd_xml, n_threads, no_chunks;
    struct stat f_xml_stat;
    off_t xml_len;

    // init multi_dfa
    FILE* f_multi_dfa = fopen(ARG_DFA, "r");
    if(!f_multi_dfa) panic("could not open multi_dfa file.");
    multidfa_init(f_multi_dfa, &multi_dfa);
    fclose(f_multi_dfa);

    fd_xml = open(ARG_XML, O_RDONLY);
    if(fd_xml < 0) panic("could not open xml file.");
    fstat(fd_xml, &f_xml_stat);
    xml_len = (size_t) f_xml_stat.st_stize;

    char* xml_buf =
    mmap(   0,                   // address
            xml_len,             // length
            PROT_READ,           // prot flags
            0,                   // 
            fd_xml,              // file decriptor
            0);                  // offset in file
    if(xml_buf == MAP_FAILED) panic("could not map file");
    

    n_threads = multi_dfa.no_dfa;

    // chunk xml stream
    char** chunks[n_threads+1];
    no_chunks = bufsplit_split_xml_stream(xml_buf, xml_len, n_threads, chunks);

    // we want at least as many (meaningful) chunks as threads
    if(no_chunks != no_threads) panic("your xml is too small!");
   
    // initialize token stream
	Tokenstream token_stream[n_threads];
	for(int i = 0; i < n_threads; ++i)
	{
		create_tokenstream(&token_stream[i], (4096) / sizeof(Token));

		if(i != n_threads -1)
		{
			token_stream[i].next = &token_stream[i+1];
		}
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

		parser = alloc_parser(chunk_begin, chunk_end);
		init_parser(parser, map);
		Tokenstream * ts_iter = t_streams + tid; 
		Token * current = get_new_token_pointer(&ts_iter);
		while(get_next_token(parser, current) == 1)
		{
			current = get_new_token_pointer(&ts_iter);
		}
		free(parser);
	}
	
    destroy_map(map);

    // run dfas
    // start measurements

#pragma omp parallel num_threads(n_threads) firstprivate(ts)
    {
        Tokenstream* ts_iter = token_stream;
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
    
    Tokenstream* ts_iter = token_stream;
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
    destroy_tokenstream(token_stream);
    // tobias will write a function
}
