cmake_minimum_required(VERSION 3.3)
project(DPHPC15)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp compression/SurpressClosingDeflator.cpp compression/SurpressClosingDeflator.h
        parser/token_list.h multi_dfa/MultiDFA.cpp multi_dfa/MultiDFA.h)
add_executable(DPHPC15 ${SOURCE_FILES})