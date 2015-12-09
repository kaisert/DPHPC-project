#!/bin/sh
icc -fopenmp -std=c++11 -O3 -mmic main.cpp parser/token_list.cpp parser/lexer.cpp parser/map.cpp parser/parser.cpp multi_dfa/MultiDFA.cpp chunker/bufsplit.cpp timing/GlobalTicToc.cpp -o test.mic
