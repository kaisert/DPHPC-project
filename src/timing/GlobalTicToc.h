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

class GlobalTicToc {
    using timepoint_t = time_point<high_resolution_clock>;
public:
    GlobalTicToc() {};

    void start_phase();
    void stop_phase(string phase_name);

    template<typename T>
    unsigned long get_phase_period(string phase_name) {
        auto phase_times = _phases[phase_name];
        unsigned long duration = duration_cast<T>(phase_times.second - phase_times.first).count();
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

    map<string, pair<timepoint_t, timepoint_t>>::iterator begin() {
        return _phases.begin();
    };

    map<string, pair<timepoint_t, timepoint_t>>::iterator end() {
        return _phases.end();
    };
private:
    time_point<high_resolution_clock> _cur_phase;
    map<string, pair<timepoint_t, timepoint_t>> _phases;
};


#endif //DPHPC15_GLOBALTICTOC_H
