#include <omp.h>
#include "Chunker.c"
#include "Chunker.c"


int main(int argc, char** argv)
{

    chunker_init("/mnt/hostfs/bakadi/test");

    chunker_load_file_content();

    char ** chunks = chunker_compute_chunks(-1);
    
    free(chunks);
    chunker_cleanup();

    return 0;
}
