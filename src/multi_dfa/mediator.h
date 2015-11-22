#ifndef MEDIATOR_H
#define MEDIATOR_H
#include<omp.h>
#include"../chunker/chunker.h"
#include"token_list.h"
#include"map.h"
#include"parser.h"
#include"tokenstream.h"

extern Tokenstream * parse_file(const char *, const char*, int);

#endif
