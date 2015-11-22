#include <omp.h>
#include "../chunker.h"


int main(int argc, char** argv)
{

    chunker_init("/mnt/hostfs/bakadi/test");

    chunker_load_file_content();

    int num_chunks = -1;
    char ** chunks = chunker_compute_chunks(&num_chunks);

    int i;
    for (i=1; i<=num_chunks;++i)
        printf("chunk %d ends at %ld\n", i, (long) (chunks[i]-chunks[0]) );
    
    free(chunks);
    chunker_cleanup();

    return 0;
}
