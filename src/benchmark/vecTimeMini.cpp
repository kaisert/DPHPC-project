#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "tictoc.cpp"


using namespace std;


#define MB (1024L*1024)
#define GB (1024L*1024*1024)

/* used test settings */
const bool PRINT_READ_SIZE_CHANGE_WARNING = false;
const int VECTORIZATION_FACTOR = 16; // number of 32bit integers to vectorize

// vectorization test
const long long MAX_SIZE = 4L*GB;
const long long NUM_REPETITIONS = 10;


void do_experiment(char* buf, bool use_vectorized_code) {

    TicToc benchmark;
    benchmark.start();

    long long start = 0;
    long long end = MAX_SIZE;
    

    char msg[100];
    uint32_t * buf32 = reinterpret_cast<uint32_t*>(buf);
    uint32_t crc = 0;


     
    for (long long e = 0;e<NUM_REPETITIONS;++e) {
        crc += e;
        if (use_vectorized_code) {
            uint32_t vcrc [VECTORIZATION_FACTOR];
            memset(vcrc,0,sizeof(vcrc[0])*VECTORIZATION_FACTOR);

            long long i;
            for (i = start/4;i<=end/4-VECTORIZATION_FACTOR;i+=VECTORIZATION_FACTOR) {
                for (int vec=0;vec<VECTORIZATION_FACTOR;++vec) {
                    vcrc[vec] += buf32[i+vec];
                }
            }

            for (int o=0;o<VECTORIZATION_FACTOR;++o)
                crc += vcrc[o];


            if (i < end/4) {
                for (int o=max(i,0LL);o<end/4;++o)
                    crc += buf32[o];
            }
        } else {
            for (long long i = start/4;i<end/4;++i) {
                crc += buf32[i];
            }
        }

        sprintf(msg, (use_vectorized_code?"vecCRC 0x%x computed":"unvecCRC 0x%x computed"), crc);
    }
    benchmark.measure(msg);
    benchmark.printSummary();
}

int main(int argc, char** argv)
{

    char* buf = new char[MAX_SIZE];
    buf[0] = 42;

    if (buf == NULL) {
        printf("Unable to allocate buffer of size %lld bytes\n",MAX_SIZE);
        return 1;
    }

    for (long long i=1;i<MAX_SIZE;++i)
        buf[i] = buf[i-1]+1;
    



    #pragma omp barrier
    do_experiment(buf, false);

    buf[0] = buf[256];
    printf("\n");

    #pragma omp barrier
    do_experiment(buf, true);


    delete [] buf;

    return 0;
}
