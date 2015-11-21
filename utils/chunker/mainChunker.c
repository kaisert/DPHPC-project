#include <omp.h>
#include "chunker.c"


int main(int argc, char** argv)
{

    chunker_init("/mnt/hostfs/bakadi/test");

    chunker_load_file_content();

    int num_chunks = -1;
    char ** chunks = chunker_compute_chunks(&num_chunks);
    
    free(chunks);
    chunker_cleanup();

    return 0;
}
