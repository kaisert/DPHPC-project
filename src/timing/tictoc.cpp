#include "tictoc.h"

using namespace std;
using namespace std::chrono;


TicToc::TicToc() {
    threadID = -1;
}


TicToc::TicToc(int threadID) {
    TicToc::threadID = threadID;
}


void TicToc::start() {
    msg.clear();
    times.clear();
    times.push_back(high_resolution_clock::now());
}


void TicToc::measure(string message) {
    times.push_back(high_resolution_clock::now()); 

    msg.push_back(message);
}


void TicToc::printSummary() {
    time_point<high_resolution_clock> start = times.front();
    times.pop_front();

    for (list<time_point<high_resolution_clock>>::iterator i = times.begin(); i != times.end(); ++i) {
        
        long int t = duration_cast<nanoseconds>(*i-start).count();

        char buf[1000];
        if (threadID >= 0)
            sprintf(buf, "At %ld ns (~%ld ms): %s in thread %d.\n", t, t/1000000, msg.front().c_str(), threadID);
        else
            sprintf(buf, "At %ld ns (~%ld ms): %s.\n", t, t/1000000, msg.front().c_str());

        cout << buf; //for (some) thread-safety

        msg.pop_front();
    }
}

