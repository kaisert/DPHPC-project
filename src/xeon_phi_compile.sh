#!/bin/sh
icc -fopenmp -std=c++11 -mmic -Wall -qopt-report3 -qopt-report-phase=vec -O3 main.cpp parser/token_list.cpp parser/lexer.cpp parser/map.cpp parser/parser.cpp multi_dfa/MultiDFA.cpp chunker/bufsplit.cpp -o test.mic
