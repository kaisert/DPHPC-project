#include<iostream>
#include<sstream>
#include<cassert>
#include<chrono>
#include<algorithm>
#include "../../src/timing/GlobalTicToc.h"

using namespace std;

template<typename T, size_t Bytes, size_t alignment>
struct BWTest {
    static constexpr size_t elem_size = sizeof(T);

    BWTest(GlobalTicToc &gtt): _gtt(gtt) {
        _alloc_size = Bytes/elem_size + alignment/elem_size;
        _buf_alloc = new T[_alloc_size];
        _buf_aligned = _buf_alloc;
        _alloc_space = _alloc_size * elem_size;
        
        if(alignment != 1) {
            void* buf = reinterpret_cast<void*>(_buf_alloc);
            _buf_aligned = reinterpret_cast<T*>(std::align(alignment, Bytes, buf,
                        _alloc_space));
        }
        _aligned_size = min(_alloc_space/elem_size, Bytes/elem_size);
    }

    string _title(string title) {
        ostringstream s;
        s << "byte size: " << elem_size << ", Bytes: " << Bytes << ": " << title;
        return s.str();
    }

    void seq_write_scalar() {
        if(!_buf_aligned) { return; }
        LocalTicToc ltt(_gtt, _title(__FUNCTION__));
        
        for(size_t i = 0; i < _aligned_size; ++i) {
            _buf_aligned[i] = static_cast<T>(i);
        }
    }

    void seq_write_vec() {
        if(!_buf_aligned) { return; }
        LocalTicToc ltt(_gtt, _title(__FUNCTION__));

        T v[8];

        for(int k = 0; k < 8; ++k) {
            v[k] = 0;
        }

        for(size_t i = 0; i < _aligned_size; i += 8) {
            for(int k = 0; k < 8; ++k) {
                _buf_aligned[i+k] = v[k];
            }

            for(int k = 0; k < 8; ++k) {
                v[k]++;
            }
        }
    }

    T seq_read_scalar() {
        if(!_buf_aligned) { return 0; }
        LocalTicToc ltt(_gtt, _title(__FUNCTION__));
        
        T res;
        for(size_t i = 0; i < _aligned_size; ++i) {
            res += _buf_aligned[i];
        }

        return res;
    }

    T seq_read_vec() {
        if(!_buf_aligned) { return 0; }
        LocalTicToc ltt(_gtt, _title(__FUNCTION__));

        T v[8];
        for(int k = 0; k < 8; ++k) {
            v[k] = 0;
        }
        
        for(size_t i = 0; i < _aligned_size; i += 8) {
            for(int k = 0; k < 8; ++k) {
                v[k] = _buf_aligned[i+k];
            }
        }

        T sum = 0;
        for(int k = 0; k < 8; ++k) {
            sum += v[k];
        }
        return sum;
    }

    ~BWTest() {
        delete[] _buf_alloc;
    }

    T* _buf_alloc;
    T* _buf_aligned;
    size_t _alloc_size;
    size_t _alloc_space;
    size_t _aligned_size;
    GlobalTicToc& _gtt;

    struct LocalTicToc {
        LocalTicToc(GlobalTicToc& gtt, string name): _gtt(gtt), _name(name) {
            _gtt.start_phase();
        }

        ~LocalTicToc() {
            _gtt.stop_phase(_name);
        }

        string _name;
        GlobalTicToc& _gtt;
    };
};

const size_t GB = static_cast<size_t>(1)<<30;
const size_t MB = static_cast<size_t>(1)<<20;

constexpr size_t n_runs = 16;

template<typename T, size_t sz, size_t m>
void testrun(BWTest<T, sz, m>& bwtest) {
    T res = 0;
    long long sumread = 0, sumread_vec = 0, sumwrite = 0;
    bwtest.seq_write_vec();
    for( int n = 0; n < n_runs; n++ ) {
        res = bwtest.seq_read_scalar();
        sumread += bwtest._gtt.template get_phase_period<chrono::milliseconds>(*(bwtest._gtt.end()-1));
    }

    for( int n = 0; n < n_runs; n++ ) {
        res = bwtest.seq_read_vec();
        sumread_vec += bwtest._gtt.template get_phase_period<chrono::milliseconds>(*(bwtest._gtt.end()-1));
    }

    cout << "sizeof(): " << sizeof(T) << 
        ", Bytes: " << sz << ", Alignment: " << m <<
        ", Aligned Size: " << bwtest._aligned_size << ", Average scalar (ms): "
        << sumread/n_runs << ", Avg. vec (ms): " << sumread_vec/n_runs << endl;
}

int main(int argc, char* argv[]) {
    GlobalTicToc gtt;
    { 
        BWTest<long long, 2048*MB, 1> bwtest(gtt);
        testrun(bwtest);
    }

    //gtt.summary<chrono::nanoseconds>(cout);
    return 0;
}
