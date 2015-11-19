#include <iostream>
#include <chrono>
#include <list>
#include <string>

using namespace std;
using namespace std::chrono;


class TicToc
{
    protected:
        list<time_point<high_resolution_clock>> times;
        list<string> msg; //list of messages to identify times
        int threadID; //non-negative number identifying the thread

    public:
        TicToc() {
            threadID = -1;
        }

        /**
         * Initialize the benchmarking with a non-negative unique thread ID
         */
        TicToc(int threadID) {
            TicToc::threadID = threadID;
        }

        
        /**
         * Starts/resets the measurement(s)
         */
        void start() {
            msg.clear();
            times.clear();
            times.push_back(high_resolution_clock::now());
        }

        /**
         * Measures timing to an accuracy of about 5 microseconds (per call)
         *
         * message      String identifying the measurement point
         */
        void stop(string message) {
            times.push_back(high_resolution_clock::now()); 

            msg.push_back(message);
        }

        /**
         * Prints a default summary of all measurement times 
         */
        void printSummary() {
            
            list<time_point<high_resolution_clock>>::iterator i = times.begin();
            time_point<high_resolution_clock> start = times.front();


            list<string>::iterator msgiterator = msg.begin();
            for (++i; i != times.end(); ++i) {
                long int t = duration_cast<nanoseconds>(*i-start).count();
                char buf[1000];
                if (threadID >= 0)
                    sprintf(buf, "At %ld ns (~%ld ms): %s in thread %d.\n", t, t/1000000, msgiterator->c_str(), threadID);
                else
                    sprintf(buf, "At %ld ns (~%ld ms): %s.\n", t, t/1000000, msgiterator->c_str());


                printf("%s", buf); //for (some) thread-safety

                ++msgiterator;
            }
        }
};
