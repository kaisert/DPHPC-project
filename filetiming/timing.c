#include <omp.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include "tictoc.h"


//Activate the define to test without potentially sequential prefetching
//
// #define ONLYPARALLEL

//chunk size for testing
const long fileSize = 2L*(1024*1024*1024);//16*(1024*1024);// /* bytes */
const int bufsize = 2*1024*1024;


int main(int argc, char** argv)
{
    struct TicToc benchmark = TicToc_create(-1,10);

#ifndef ONLYPARALLEL
    FILE* fp = fopen("/mnt/hostfs/bakadi/test", "r"); //file on Xeon Phi
    if(!fp) {
        perror("File opening failed for test file");
        return EXIT_FAILURE;
    }
 

    TicToc_start(&benchmark);

    long int c;
    uint32_t crcSequential = 0;
    long int count = 0;
    char* buf = (char*) calloc(bufsize, sizeof(char));
    while ((c = fread(buf, sizeof(buf[0]), bufsize, fp)) != 0) {
        count += c;
            
        for (int i=0;i<c;i+=4)
            crcSequential += *((uint32_t*) (&buf[i]));
    }

    
    TicToc_measure(&benchmark, "file read completed");
    printf("%ld bytes read from file sequentially.\n", count);
    TicToc_print_summary(&benchmark);


    if (count != fileSize)
    {
        printf("\nWARNING: Specified fileSize does not match with the real file.\n\n");
        //fileSize = count;
    }

    if (ferror(fp))
        puts("I/O error while reading");
 
    fclose(fp);
#endif


    /* parallel file read test */

    int nthreads,tid;

    uint32_t crcParallel = 0;


    TicToc_start(&benchmark);
    #pragma omp parallel private(nthreads, tid) shared(crcParallel)
    {
        tid = omp_get_thread_num();
        nthreads = omp_get_num_threads();


        long int chunkSize = fileSize/nthreads;

        if (fileSize % nthreads != 0)
            ++chunkSize;

        if (tid == 0) {
            if (fileSize % nthreads != 0)
                printf("chunkSize has been ceiled to %ld to read the whole file\n", chunkSize);

            if (chunkSize % 4 != 0)
                printf("chunkSize needs to be a multiple of 4 for the CRCs to be computed correctly.\n");
                //e.g. use a 2GiB file and set omp num_threads(256)
            
            printf("Parallel file read of %ld bytes (chunks of %ld each for %d threads) started.\n", fileSize, chunkSize, nthreads);
        }

        struct TicToc parallelBenchmark = TicToc_create(tid,10);

        FILE* fp = fopen("/mnt/hostfs/bakadi/test", "r");
        if(fp) {
            long int c;
            uint32_t crcLocal = 0;
            long int count = 0;
            char* buf = (char*) calloc(bufsize, sizeof(char));

            
            TicToc_start(&parallelBenchmark);
            fseek(fp, tid*chunkSize, SEEK_SET);
            while ((c = fread(buf, sizeof(buf[0]), bufsize, fp)) != 0 && count < chunkSize) {
                count += c;
                    
                for (int i=0;i<c;i+=4)
                    crcLocal += *((uint32_t*) (&buf[i]));
            }

            
            TicToc_measure(&parallelBenchmark, "file read completed");
            //printf("%ld bytes read from file in thread %i.\n", count, tid);
            TicToc_print_summary(&parallelBenchmark);
            TicToc_free(&parallelBenchmark);

            #pragma omp critical
            {
                crcParallel += crcLocal;
                //printf("Thread %i computed its chunk CRC: 0x%x\n", tid, crcLocal);
            }
            
            if (count < chunkSize)
                printf("Thread %i read less than chunk size: %ld of %ld bytes.\n  (last incomplete chunk or seek() beyond EOF?)\n", tid, count, chunkSize);
                

            if (ferror(fp))
                puts("I/O error while reading");
         
            fclose(fp);
        } else {
            perror("File opening failed.\n");
        }
    }

    TicToc_measure(&benchmark, "total time for parallel file read");
    TicToc_print_summary(&benchmark);
    TicToc_free(&benchmark);

#ifdef ONLYPARALLEL
    printf("\nParallel CRC: 0x%x\n", crcParallel);
#else     
    if (crcSequential == crcParallel)
        printf("\nBoth CRCs matched: 0x%x\n", crcSequential);
    else
    {
        printf("\nCRCs did not match:\n0x%x (sequential) vs. \n0x%x (parallel)\n", crcSequential, crcParallel);
        printf("Please make sure that:\n");
        printf("- the file size is divisible by the computed chunk size\n");
        printf("- chunk and file size is a multiple of 4\n");
    }
#endif

    return 0;
}
