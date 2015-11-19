#include <omp.h>
#include "../../filetiming/tictoc.cpp"
#include "Chunker.cpp"

using namespace std;

int main(int argc, char** argv)
{
    TicToc benchmark;

    bool chunker_error;
    benchmark.start();

    Chunker chunker("/mnt/hostfs/bakadi/test", chunker_error);
    if (chunker_error)
        return 1;

    chunker.load_file_content();
    benchmark.stop("file read done");

    char ** chunks = chunker.compute_chunks(-1);
    benchmark.stop("chunks computed");
    
    delete [] chunks;
    benchmark.printSummary();

    return 0;
}
