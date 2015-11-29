#ifndef TICTOC_H
#define TICTOC_H

#include <iostream>
#include <chrono>
#include <list>


class TicToc
{
    protected:
        std::list<std::chrono::time_point<std::chrono::high_resolution_clock>> times;
        std::list<std::string> msg; //list of messages to identify times
        int threadID; //non-negative number identifying the thread

    public:
        TicToc();


        /**
         * Initialize the benchmarking with a non-negative unique thread ID
         */
        TicToc(int threadID);

        
        /**
         * Starts/resets the measurement(s)
         */
        void start();


        /**
         * Measures timing to an accuracy of about 5 microseconds (per call)
         *
         * message      String identifying the measurement point
         */
        void measure(std::string message);


        /**
         * Prints a default summary of all measurement times 
         */
        void printSummary();
};

#endif
