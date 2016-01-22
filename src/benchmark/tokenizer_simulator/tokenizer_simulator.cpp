#ifdef _OPENMP
#include<omp.h>
#else
#define omp_get_thread_num() 0
#endif

#include <iostream>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <cassert>

using namespace std;

#define MB (static_cast<size_t>(1024*1024))

void calc_offsets(vector<size_t> &offsets, size_t total_size, size_t chunks) {
    assert(chunks > 0);
    size_t floor_size = total_size/chunks;
    for(size_t i = 0; i < chunks; i++) {
        offsets.push_back(i*floor_size);
    }
    offsets.push_back(total_size);
}

// args: <total_size> <output_ratio> <runs>

// <total_size>: total size of the memory to be read (in MiB)
//
// <output_ratio>: after each <output_ratio> bytes some output will be generated 
//                 and written to memory (equivalent amount/location as with the
//                 real token_stream and offset_stream)
//
// <runs> the number of runs
int main(int argc, char* argv[]) {
    vector<size_t> offsets;
    vector<unsigned long> results;

    if(argc < 4) {
        cout << "too few arguments, see code ..." << endl;
        return -1;
    }

    int n_threads = omp_get_max_threads();
    size_t total_size = atol(argv[1]) * MB;
    size_t output_ratio = atol(argv[2]);
    size_t runs = atol(argv[3]);
    vector<vector<short>> token_stream(n_threads);
    vector<vector<size_t>> offset_stream(n_threads);

    results.assign(runs, 0);
    calc_offsets(offsets, total_size, static_cast<size_t>(n_threads));

    char* stream = static_cast<char*>(malloc(total_size));
    if(!stream) {
        cout << "failed to initialize memory" << endl;
        return -2;
    }

    // init memory
    for(size_t i = 0; i < total_size; ++i) {
        stream[i] = (char) i;
    } 

    for(size_t run = 0; run < runs; ++run) {
        // start timer
        auto _start = chrono::high_resolution_clock::now();
        auto& token_stream_ref = token_stream;
        auto& offset_stream_ref = offset_stream;
#pragma omp parallel num_threads(n_threads) \
        shared(n_threads, offsets, stream, token_stream_ref, offset_stream_ref)
        {
            int tid = omp_get_thread_num() % n_threads;
            size_t end = offsets.at(tid+1);
            short sum = 0;
            auto& my_token_stream = token_stream_ref.at(tid);
            auto& my_offset_stream = offset_stream_ref.at(tid);
            size_t idx = offsets.at(tid);
            int stride_offset = idx % output_ratio;

            for(size_t idx = offsets.at(tid); idx < end; ++idx) {
                sum += stream[idx];
                if(output_ratio == stride_offset) {
                    my_token_stream.push_back(static_cast<short>(sum));
                    my_offset_stream.push_back(static_cast<size_t>(sum));
                    stride_offset = 0;
                }
                stride_offset++;
            }

        }
        auto _stop = chrono::high_resolution_clock::now();
        unsigned long duration =
            chrono::duration_cast<chrono::milliseconds>(_stop - _start).count();
        results.at(run) = duration;
    }

    cout << "threads:" << n_threads << ",total_size:" << total_size << endl;

    // output results
    if(results.size() > 0) {
        cout << results.at(0);
    }

    for(int i = 1; i < runs; ++i) {
        cout << ", " << results.at(i);
    }

    cout << endl;
}
