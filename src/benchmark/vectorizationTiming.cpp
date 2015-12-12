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
#define CODE_UNVECTORIZED

/* used test settings */
const bool PRINT_READ_SIZE_CHANGE_WARNING = false;
const int VECTORIZATION_FACTOR = 16; // number of 32bit integers to vectorize

// vectorization test
const long long MAX_SIZE = 8L*GB+1;
const long long MIN_SUPERCHUNKS = 1;
const long long MAX_SUPERCHUNKS = 4;
const long long READ_SIZE = 32L*GB;
const long long SET_THREADS = 60;
const long long INIT_STRIDE = 128L*MB/60;
const long long NUM_REPETITIONS = 10;
#define next_stride(x) ((x)*2L)
// */


/*/ huge memory span 8GB => big_strides_simulating_tokenizer.csv
const long long MAX_SIZE = 8L*GB+10240;
const long long MIN_SUPERCHUNKS = 1;
const long long MAX_SUPERCHUNKS = 64;
const long long READ_SIZE = 32L*GB;
const long long SET_THREADS = 60;
const long long INIT_STRIDE = 128L*MB/60;
const long long NUM_REPETITIONS = 1;
#define next_stride(x) ((x)*2)
// */

/*/ huge memory span 10GB => big_strides_read_all.csv
const long long MAX_SIZE = 10L*GB;
const long long MIN_SUPERCHUNKS = 1;
const long long MAX_SUPERCHUNKS = 8;
const long long READ_SIZE = 32L*GB;
const long long SET_THREADS = 60;
const long long INIT_STRIDE = 16L*MB;
const long long NUM_REPETITIONS = 1;
#define next_stride(x) ((x)+1048573)
// */


/*/ tiny memory span 512MB, high resolution => tiny_strides.csv
const long long MAX_SIZE = 512L*MB;
const long long MIN_SUPERCHUNKS = 1;
const long long MAX_SUPERCHUNKS = 1;
const long long READ_SIZE = 4L*MB;
const long long SET_THREADS = 60;
const long long INIT_STRIDE = 8388593;
const long long NUM_REPETITIONS = 6;
#define next_stride(x) ((x)+563)
// */


/*/ small memory span 1GB => small_strides.csv
const long long MAX_SIZE = 1L*GB;
const long long MIN_SUPERCHUNKS = 1;
const long long MAX_SUPERCHUNKS = 1;
const long long READ_SIZE = 4L*MB;
const long long SET_THREADS = 60;
const long long INIT_STRIDE = 8388593;
const long long NUM_REPETITIONS = 6;
#define next_stride(x) ((x)+33797)
// */


/*/ huge memory span 10GB => big_strides.csv
const long long MAX_SIZE = 10L*GB;
const long long MIN_SUPERCHUNKS = 1;
const long long MAX_SUPERCHUNKS = 8;
const long long READ_SIZE = 8L*MB;
const long long SET_THREADS = 60;
const long long INIT_STRIDE = 16L*MB;
const long long NUM_REPETITIONS = 3;
#define next_stride(x) ((x)+1048573)
// */

void do_experiment(char* buf, long long stride, bool use_vectorized_code) {

    TicToc benchmark;

    while (stride*SET_THREADS*MIN_SUPERCHUNKS < MAX_SIZE) {

        long long cur_max_superchunks = MIN_SUPERCHUNKS-1;

        while (stride*SET_THREADS*(cur_max_superchunks+1) < MAX_SIZE && cur_max_superchunks+1 <= MAX_SUPERCHUNKS)
            ++cur_max_superchunks;

        benchmark.start();

        for (long long a=0;a<cur_max_superchunks;++a) {
            const long long cur_sc = a;
            #pragma omp parallel num_threads(SET_THREADS)
            {
                int tid = omp_get_thread_num();
           
                if (PRINT_READ_SIZE_CHANGE_WARNING && tid == 0 && stride < READ_SIZE)
                    printf("Stride is smaller than READ_SIZE, adapting...\n");
             
                TicToc parallelBenchmark(tid);
                parallelBenchmark.start();

                long long blocksize = (READ_SIZE<stride?READ_SIZE:stride);

                long long start = cur_sc*SET_THREADS*stride + stride*tid;
                if (start % 4 != 0) {
                    start -= (start % 4); //round down to int boundary
                }
                long long end = start+blocksize;
                

                char msg[100];
                uint32_t * buf32 = reinterpret_cast<uint32_t*>(buf);
                uint32_t crc = 0;


                if (end < MAX_SIZE) {

                    for (long long e = 0;e<NUM_REPETITIONS;++e) {
                        crc += e;
                        if (use_vectorized_code) {
                            uint32_t vcrc [VECTORIZATION_FACTOR];
                            memset(vcrc,0,sizeof(vcrc[0])*VECTORIZATION_FACTOR);
                            for (long long i = start/4;i<end/4;i+=VECTORIZATION_FACTOR) {

                                if (i+VECTORIZATION_FACTOR <= end/4) {
                                    for (int vec=0;vec<VECTORIZATION_FACTOR;++vec) {
                                        vcrc[vec] += buf32[i+vec];
                                    }
                                }

                                if (i+VECTORIZATION_FACTOR >= end/4) {
                                    for (int o=0;o<VECTORIZATION_FACTOR;++o)
                                        crc += vcrc[o];

                                    if (i+VECTORIZATION_FACTOR > end/4) {
                                        for (int o=i;o<end/4;++o)
                                            crc += buf32[o];
                                    }

                                    break;
                                }
                            }
                        } else {
                            for (long long i = start/4;i<end/4;++i) {
                                crc += buf32[i];
                            }
                        }
                    }

                    //CRC is always the same for big power of two strides/READ_SIZE
                    sprintf(msg, "CRC 0x%x computed", crc);
                } else
                    sprintf(msg, "ERROR: CRC beyond MAX_SIZE");

                parallelBenchmark.measure(msg);
                if (crc == 0x42424242)
                    parallelBenchmark.printSummary();
            }


            if (a+1 >= MIN_SUPERCHUNKS) {
                char bench_msg[300];
                sprintf(bench_msg, (use_vectorized_code?
                    "Read vectorized %lld bytes with a stride of %lld (%lld threads, %lld superchunk(s), full size=%lld)":
                    "Read unvectorized %lld bytes with a stride of %lld (%lld threads, %lld superchunk(s), full size=%lld)"),
                         READ_SIZE, stride, SET_THREADS, (a+1), stride*SET_THREADS*(a+1));

                benchmark.measure(bench_msg);
            }

        }
        benchmark.printSummary();

        stride = next_stride(stride);
    }
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
    do_experiment(buf, INIT_STRIDE, false);

    buf[0] = buf[256];
    printf("\n\n");

    #pragma omp barrier
    do_experiment(buf, INIT_STRIDE, true);


    delete [] buf;

    return 0;
}
