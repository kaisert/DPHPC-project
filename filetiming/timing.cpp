#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "tictoc.cpp"


using namespace std;


FILE* getTestFile() {
    FILE* fp = fopen("/mnt/hostfs/bakadi/test.bin", "r"); //file on Xeon Phi
    if(!fp) {
        fp = fopen("test", "r"); //local fallback file
    }

    return fp;
}


//Activate the define to test without potentially sequential prefetching
//
// #define ONLYPARALLEL

//chunk size for testing
const long fileSize = 2L*(1024*1024*1024);//16*1024*1024;// /* bytes */
const int bufsize = 512*1024;
//TODO 2MiB (or even below) might segfault the stack on our server -> use pointer & new char[]



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
    uint32_t crcSequential = 0;
    long int count = 0;
    char buf[bufsize];
    while ((c = fread(&buf[0], sizeof(buf[0]), bufsize, fp)) != 0) {
        count += c;
            
        for (int i=0;i<c;i+=4)
            crcSequential += *(reinterpret_cast<uint32_t*>(&buf[i]));
    }

    


    benchmark.stop("file read completed");
    printf("%ld bytes read from file sequentially.\n", count);
    benchmark.printSummary();


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

    benchmark.start();
    #pragma omp parallel private(nthreads, tid) shared(crcParallel)
    {
        tid = omp_get_thread_num();
        nthreads = omp_get_num_threads();


        long int chunkSize = fileSize/nthreads;

        if (fileSize % nthreads != 0)
            ++chunkSize;

        if (tid == 0) {
            if (chunkSize % 4 != 0)
                printf("chunkSize needs to be a multiple of 4 for the CRCs to be computed correctly.\n");

            if (fileSize % nthreads != 0)
                printf("chunkSize has been adapted to %ld.\n", chunkSize);
            
            printf("Parallel file read of %ld bytes (chunks of %ld each for %d threads) started.\n", fileSize, chunkSize, nthreads);
        }


        TicToc parallelBenchmark(tid);

        FILE* fp = getTestFile();
        if(fp) {
            parallelBenchmark.start();
            fseek(fp, tid*chunkSize, SEEK_SET);
            
            int c;
            uint32_t crcLocal = 0;
            long int count = 0;
            char buf[bufsize];
            while ((c = fread(&buf[0], sizeof(buf[0]), bufsize, fp)) != 0 && count < chunkSize) {
                count += c;
                    
                for (int i=0;i<c;i+=4)
                    crcLocal += *(reinterpret_cast<uint32_t*>(&buf[i]));
            }

            
            parallelBenchmark.stop("file read completed");
            //printf("%ld bytes read from file in thread %i.\n", count, tid);
            parallelBenchmark.printSummary();

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

    benchmark.stop("total time for parallel file read");
    benchmark.printSummary();

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
        printf("- number of bytes read are the same\n");
    }
#endif

    return 0;
}
