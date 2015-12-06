#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "tictoc.cpp"


using namespace std;

/* used test settings */


// tiny memory span 512MB, tiny strides
const long long MAX_SIZE = 512L*1024*1024;
const long long MIN_SUPERCHUNKS = 1;
const long long MAX_SUPERCHUNKS = 1;
const long long READ_SIZE = 4L*1024*1024;
const long long SET_THREADS = 60;
const long long INIT_STRIDE = 8388593;
const long long NUM_REPETITIONS = 6;
#define next_stride(x) ((x)+563)
// */


/*/ small memory span 1GB, small strides
const long long MAX_SIZE = 1L*1024*1024*1024;
const long long MIN_SUPERCHUNKS = 1;
const long long MAX_SUPERCHUNKS = 1;
const long long READ_SIZE = 4L*1024*1024;
const long long SET_THREADS = 60;
const long long INIT_STRIDE = 8388593;
const long long NUM_REPETITIONS = 6;
#define next_stride(x) ((x)+33797)
// */


/*/ huge memory span 10GB, superchunks and big strides
const long long MAX_SIZE = 10L*1024*1024*1024;
const long long MIN_SUPERCHUNKS = 1;
const long long MAX_SUPERCHUNKS = 8;
const long long READ_SIZE = 8L*1024*1024;
const long long SET_THREADS = 60;
const long long INIT_STRIDE = 16L*1024*1024;
const long long NUM_REPETITIONS = 3;
#define next_stride(x) ((x)+1048573)
// */


int main(int argc, char** argv)
{
    TicToc benchmark;


    char* buf = new char[MAX_SIZE];
    buf[0] = 42;

    if (buf == NULL) {
        printf("Unable to allocate buffer of size %lld bytes\n",MAX_SIZE);
        return 1;
    }

    for (long long i=1;i<MAX_SIZE;++i)
        buf[i] = buf[i-1]+1;
    

    long long stride = INIT_STRIDE;


    #pragma omp barrier
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
             
                TicToc parallelBenchmark(tid);
                parallelBenchmark.start();
           
                if (tid == 0 && stride < READ_SIZE)
                    printf("Stride is smaller than READ_SIZE, adapting...\n");

                long long blocksize = (READ_SIZE<stride?READ_SIZE:stride);

                uint32_t crc = 0;
                long long start = cur_sc*SET_THREADS*stride + stride*tid;
                long long end = start+blocksize;
                

                char msg[100];
                if (end+3 < MAX_SIZE) {
                    for (long long e = 0;e<NUM_REPETITIONS;++e)
                        for (long long i = start;i<end;i+=4) {
                            if (i+3 >= MAX_SIZE)
                                printf("out of bounds in thread %d with i=%lld\n", tid, i);
                            else {
                                char* t1 = &(buf[i]);
                                uint32_t* t2 = reinterpret_cast<uint32_t*>(t1);

                                if ((long) t2 < (long) buf)
                                    printf("t2 %ld before buf %ld\n",(long) t2, (long) buf);

                                if ((long) t2 > (long) (buf+MAX_SIZE-1))
                                    printf("t2 %ld after buf %ld\n",(long) t2, (long) (buf+MAX_SIZE-1));

                                crc += *(t2);
                            }
                        }

                    //CRC is always the same for big power of two strides/READ_SIZE
                    sprintf(msg, "CRC 0x%x computed", crc);
                } else
                    sprintf(msg, "ERROR: CRC beyond MAX_SIZE");

                parallelBenchmark.measure(msg);
                //parallelBenchmark.printSummary();
            }

            if (a+1 >= MIN_SUPERCHUNKS) {
                char bench_msg[300];
                sprintf(bench_msg, 
                    "Read %lld bytes with a stride of %lld (%lld threads, %lld superchunk(s), full size=%lld)", 
                         READ_SIZE, stride, SET_THREADS, (a+1), stride*SET_THREADS*(a+1));

                benchmark.measure(bench_msg);
            }

        }
        benchmark.printSummary();

        stride = next_stride(stride);
    }

    delete [] buf;

    return 0;
}
