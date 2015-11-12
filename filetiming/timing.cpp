#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "tictoc.cpp"


using namespace std;


FILE* getTestFile() {
    FILE* fp = fopen("test", "r"); //local file
    if(!fp) {
        //file on Xeon Phi
        fp = fopen("/mnt/hostfs/bakadi/test", "r");
    }

    return fp;
}

//Activate the define to test without potentially sequential prefetching
//
// #define ONLYPARALLEL

//chunk size for testing
const long chunkSize = 2L*(1024*1024*1024)/240; //4*1024*1024; /* bytes */


int main(int argc, char** argv)
{
    TicToc benchmark;

#ifndef ONLYPARALLEL
    FILE* fp = getTestFile();
    if(!fp) {
        perror("File opening failed");
        return EXIT_FAILURE;
    }
 

    benchmark.start();

    int c;
    long int count = 0;    
    const int bufsize = 2*1024*1024;
    char buf[bufsize];
    while ((c = fread(&buf[0], sizeof(buf[0]), bufsize, fp)) != 0) {
        count += c;
    }
    


    benchmark.stop("file read completed");
    printf("%ld bytes read from file sequentially.\n", count);
    benchmark.printSummary();


    if (ferror(fp))
        puts("I/O error while reading");
 
    fclose(fp);

#endif

    /* parallel file read test */

    int nthreads,tid;

    benchmark.start();
    #pragma omp parallel private(nthreads, tid)
    {
        tid = omp_get_thread_num();
        nthreads = omp_get_num_threads();

        if (tid == 0) {
            printf("Parallel file read of %ld bytes (distributed among %d threads) started.\n", chunkSize*nthreads, nthreads);
        }

        TicToc parallelBenchmark(tid);

        FILE* fp = getTestFile();
        if(fp) {
            parallelBenchmark.start();
            fseek(fp, tid*chunkSize, SEEK_SET);
            

            int c;
            long int count = 0;    
            const int bufsize = 2*1024*1024;
            char buf[bufsize];
            while ((c = fread(&buf[0], sizeof(buf[0]), bufsize, fp)) != 0 && count < chunkSize) {
                count += c;
            }

            
            parallelBenchmark.stop("file read completed");
            //printf("%ld bytes read from file in thread %i.\n", count, tid);
            parallelBenchmark.printSummary();
            
            
            if (count < chunkSize)
                printf("Thread %i read less than chunk size: %ld of %ld bytes. (seek() beyond EOF?)\n", tid, count, chunkSize);
                


            if (ferror(fp))
                puts("I/O error while reading");
         
            fclose(fp);
        } else {
            perror("File opening failed.\n");
        }
    }

    benchmark.stop("total time for parallel file read");
    benchmark.printSummary();

    return 0;
}
