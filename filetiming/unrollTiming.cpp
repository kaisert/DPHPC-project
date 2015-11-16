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
const int bufsize = 1*1024*1024;


int main(int argc, char** argv)
{
    TicToc benchmark;


    FILE* fp = getTestFile();
    if(!fp) {
        perror("File opening failed for test file");
        return EXIT_FAILURE;
    }


    benchmark.start();

    long int c;
    uint32_t crcSequential = 0;
    long int count = 0;
    char* buf = new char[fileSize];

    while ((c = fread(buf, sizeof(buf[0]), fileSize, fp)) != 0) {
        count += c;
    }
    benchmark.stop("file read done");


    uint32_t crcParallel = 0;


    #pragma omp parallel shared(crcParallel)
    {
        int tid = omp_get_thread_num();
        int nthreads = omp_get_num_threads();

        
        long end = count/nthreads*(tid+1);

        uint32_t crcLocal = 0;
        for (long i = count/nthreads*tid;i<end;i+=4)
        {
            crcLocal += *(reinterpret_cast<uint32_t*>(&buf[i]));
        }

        #pragma omp critical
        {
            crcParallel += crcLocal;
            //printf("Thread %i computed its chunk CRC: 0x%x\n", tid, crcLocal);
        }
    }

    benchmark.stop("CRC full loop completed");
    uint32_t crcParallel_unrolled = crcParallel;
    //WARNING icc performs optimizations across this line 
    //        and omp pragmas such that time measurements
    //        are not reliable.
    printf("WARNING: icc might optimize across multiple pragmas and thus influence the recorded times.\n");

    crcParallel = 0;
    #pragma omp parallel shared(crcParallel)
    {
        int tid = omp_get_thread_num();
        int nthreads = omp_get_num_threads();

        
        long end = count/nthreads*(tid+1);

        uint32_t crcLocal = 0;
        for (long i = count/nthreads*tid;i<end;i+=16)
        {
            crcLocal = crcLocal + (
(*(reinterpret_cast<uint32_t*>(&buf[i  ]))+*(reinterpret_cast<uint32_t*>(&buf[i+4])) ) +
(*(reinterpret_cast<uint32_t*>(&buf[i+8]))+*(reinterpret_cast<uint32_t*>(&buf[i+12])))
            );
        }

        #pragma omp critical
        {
            crcParallel += crcLocal;
            //printf("Thread %i computed its chunk CRC: 0x%x\n", tid, crcLocal);
        }
    }

    benchmark.stop("CRC loop_unrolled completed");


    printf("%ld bytes read from file sequentially.\n", count);
    printf("parallel CRCs (no matching necessary): 0x%x (unrolled) vs. 0x%x\n", crcParallel_unrolled, crcParallel);
    benchmark.printSummary();



    if (count != fileSize)
    {
        printf("\nWARNING: Specified fileSize does not match with the real file.\n\n");
        //fileSize = count;
    }

    if (ferror(fp))
        puts("I/O error while reading");
 
    fclose(fp);


    return 0;
}
