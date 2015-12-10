#!/bin/sh
icc -fopenmp -std=c++11 -mmic -Wall -qopt-report3 -qopt-report-phase=vec -O3 directoryTiming.cpp -o test.mic
