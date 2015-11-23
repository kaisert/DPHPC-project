#include"mediator.h"

Tokenstream * parse_file(const char *path_xml, const char *path_tokens, int n_threads)
{
	char ** chunks;
	chunker_init(path_xml);
	chunker_load_file_content();
	chunks = chunker_compute_chunks(&n_threads);
	
	Tokenstream * t_streams = malloc(n_threads * sizeof(Tokenstream));
	for(int i = 0; i < n_threads; ++i)
	{
		create_tokenstream(t_streams + i, (1024) / sizeof(Token)); //4MB per tokenstream
		if(i != n_threads -1)
		{
			t_streams[i].next = &t_streams[i+1];
		}
	}
	
	Map * map = alloc_map(path_tokens);
	print_map(map);

	int tid;
	Parser * parser;
//#pragma omp parallel num_threads(n_threads) firstprivate(map,chunks,num_chunks) private(tid, parser)
	{
//		tid = omp_get_thread_num() % n_threads;
		tid = 0;
		char * chunk_begin, * chunk_end;
		chunk_begin = chunks[tid];
		chunk_end = chunks[tid + 1];

		parser = alloc_parser(chunk_begin, chunk_end);
		init_parser(parser, map);
		Tokenstream * ts = t_streams + tid; 
		Token * current = get_new_token_pointer(&ts);
		while(get_next_token(parser, current) == 1)
		{
			current = get_new_token_pointer(&ts);
		}
	}

	return t_streams;
}
