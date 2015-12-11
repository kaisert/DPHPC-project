#include"parser.h"

void init_lexer(Lexer &lxr, char * begin, char *end)
{
	lxr.begin = begin;
	lxr.end = end;
}

Parser::Parser(char * begin, char * end, TagMap &map)
    :map(map)
{
    init_lexer(lxr, begin, end);
}