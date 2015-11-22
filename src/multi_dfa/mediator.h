#ifndef MEDIATOR_H
#define MEDIATOR_H
#include<omp.h>
#include"../chunker/chunker.h"
#include"../parser/token_list.h"
#include"../parser/map.h"
#include"../parser/parser.h"
#include"../parser/tokenstream.h"

extern Tokenstream * parse_file(const char *, const char*, int);

#endif
