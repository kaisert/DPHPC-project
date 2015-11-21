#include <omp.h>
#include "utils/chunker/chunker.c"
#include "utils/parser/map.h"
#include "utils/parser/parser.h"


int main(int argc, char** argv) {

    chunker_init("/mnt/hostfs/bakadi/test");
    chunker_load_file_content();
    int num_chunks = -1;
    char ** chunks = chunker_compute_chunks(&num_chunks);

    
	Map *map = alloc_map("todo");


    #pragma omp parallel firstprivate(map,chunks,num_chunks)
    {
		int tid = omp_get_thread_num();

        if (tid > num_chunks)
            printf("WARNING more threads than chunks.\n");
        else {
	        Parser *prs = alloc_parser(chunks[tid], chunks[tid+1]);
	        init_parser(prs, map);


	        Token t;

	        while(get_next_token(prs, &t) != 0)
	        {
		        int size = t.end - t.begin;
		        printf("thread: %d, tag: %.*s, type: %d\n", tid, size, t.begin, t.type);
	        }
        }
    }


    free(chunks);
    chunker_cleanup();

    return 0;
}


