//
// Created by Stefan Dietiker on 30/11/15.
//

#include<string>
#include<chrono>
#include<map>
#include<tuple>

using namespace std;
using namespace std::chrono;

#ifndef DPHPC15_GLOBALTICTOC_H
#define DPHPC15_GLOBALTICTOC_H

#include<vector>
#include <ostream>
#include<iostream>

class GlobalTicToc {
    using timepoint_t = time_point<high_resolution_clock>;
    using tictoc_entry_t = pair<string, pair<timepoint_t, timepoint_t> >;
public:
    GlobalTicToc() {};

    void start_phase()  {
        _cur_phase = high_resolution_clock::now();
    }

    void stop_phase(string phase_name) {
        auto _now = high_resolution_clock::now();
        _phases.push_back(make_pair(phase_name, make_pair(_cur_phase, _now)));
    }

    template<typename T>
    unsigned long get_phase_period(tictoc_entry_t& entry) {
        unsigned long duration = duration_cast<T>(entry.second.second - entry.second.first).count();
        return duration;
    }

    template<typename T>
    unsigned long get_total_time() {
        unsigned long total_time = 0;
        for(auto phase = _phases.begin(); phase != _phases.end(); ++phase) {
            total_time += duration_cast<T>(phase->second.second - phase->second.first).count();
        }
        return total_time;
    }

    vector<tictoc_entry_t >::iterator begin() {
        return _phases.begin();
    };

    vector<tictoc_entry_t >::iterator end() {
        return _phases.end();
    };

    template<typename T>
    void summary(ostream& os) {
        os << "Timing summary:" << std::endl;
        for(auto phase = begin(); phase != end(); ++phase) {
            os << phase->first << ": " << get_phase_period<T>(*phase)
            << " ms" << endl;
        }
        os << "Total Time: " << get_total_time<T>() << " ms" << endl;
    }
private:
    time_point<high_resolution_clock> _cur_phase;
    vector<tictoc_entry_t > _phases;
};


#endif //DPHPC15_GLOBALTICTOC_H
