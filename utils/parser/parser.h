#ifndef	PARSER_H
#define PARSER_H

#include<stdlib.h>
#include<stdint.h>
#include<stdio.h>

#include"lexer.h"
#include"map.h"
#include"token_list.h"

typedef struct Parser {
	Lexer lxr;
	Map map;
	Token remaining_token;
	uint16_t remaining_token_exists;
} Parser;

extern Parser * alloc_parser(char *, char *);
extern int get_next_token(Parser *, Token *);
extern void init_parser(Parser *, Map*);

#endif
