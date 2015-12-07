cmake_minimum_required(VERSION 2.8)
project(DPHPC15)

set(CMAKE_CXX_FLAGS "-fopenmp ${CMAKE_CXX_FLAGS} -O3 -std=c++11")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

set(SOURCE_FILES main.cpp compression/SurpressClosingVSizeDeflator.h compression/SupressClosingVSizeInflator.h
        parser/token_list.h parser/token_list.cpp parser/lexer.h parser/lexer.cpp parser/map.h parser/map.cpp
        parser/parser.cpp
        chunker/bufsplit.h chunker/bufsplit.cpp
        timing/GlobalTicToc.h timing/GlobalTicToc.cpp
        multi_dfa/MultiDFA.cpp multi_dfa/MultiDFA.h timing/GlobalTicToc.cpp timing/GlobalTicToc.h loader/MMapLoader.h exceptions/GeneralException.h config_local.h tokenizer/SuperChunkTokenizer.h matcher/DFAMatcher.h matcher/Match.h)
add_executable(DPHPC15 ${SOURCE_FILES})
