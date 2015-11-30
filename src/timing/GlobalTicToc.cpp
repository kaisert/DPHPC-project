//
// Created by Stefan Dietiker on 30/11/15.
//

#include "GlobalTicToc.h"

void GlobalTicToc::start_phase() {
    _cur_phase = high_resolution_clock::now();
}


void GlobalTicToc::stop_phase(string phase_name) {
    auto _now = high_resolution_clock::now();
    _phases[phase_name] = make_pair(_cur_phase, _now);
}
