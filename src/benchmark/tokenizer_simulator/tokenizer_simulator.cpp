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

// <total_size> <output_ratio> <runs>
int main(int argc, char* argv[]) {
    vector<size_t> offsets;
    vector<unsigned long> results;

    if(argc < 4) {
        cout << "too few arguments" << endl;
        return -1;
    }

    int n_threads = omp_get_max_threads();
    size_t total_size = atol(argv[1]) * MB;
    size_t output_ratio = atol(argv[2]);
    size_t runs = atol(argv[3]);
    cout << total_size<<","<<output_ratio<<","<<runs<<endl;
    vector<vector<short>> token_stream(n_threads);
    vector<vector<size_t>> offset_stream(n_threads);

    results.assign(runs, 0);
    calc_offsets(offsets, total_size, static_cast<size_t>(n_threads));

    char* stream = static_cast<char*>(malloc(total_size));
    if(!stream) {
        cout << "failed to initialize memory" << endl;
        return -1;
    }

    // init memory
    for(size_t i = 0; i < total_size; ++i) {
        stream[i] = i%256;
    } 

    for(size_t run = 0; run < runs; ++run) {
        // start timer
        auto _start = chrono::high_resolution_clock::now();
#pragma omp parallel num_threads(n_threads) shared(offsets, stream, token_stream, offset_stream)
        {
            int tid = omp_get_thread_num() % n_threads;
            size_t end = offsets.at(tid+1);
            short sum = 0;
            auto& my_token_stream = token_stream.at(tid);
            auto& my_offset_stream = offset_stream.at(tid);

            for(size_t idx = offsets.at(tid); idx < end; ++idx) {
                sum += stream[idx];
                if((idx % output_ratio) == 0) {
                    my_token_stream.push_back(static_cast<short>(sum));
                    my_offset_stream.push_back(static_cast<size_t>(sum));
                }
            }

        }
        auto _stop = chrono::high_resolution_clock::now();
        unsigned long duration =
            chrono::duration_cast<chrono::milliseconds>(_stop - _start).count();
        results.at(run) = duration;
    }

    // output results
   
    if(results.size() > 0) {
        cout << results.at(0);
    }

    for(int i = 1; i < runs; ++i) {
        cout << ", " << results.at(i);
    }

    cout << endl;
}