#!/bin/sh
icc -fopenmp -std=c++11 -mmic -Wall -O3 directoryTiming.cpp -o test.mic
