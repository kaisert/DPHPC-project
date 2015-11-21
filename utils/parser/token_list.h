#ifndef TOKEN_LIST_H
#define TOKEN_LIST_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Token {
	uint32_t type;
	char* begin;
	char* end;
} Token;

typedef struct TokenList {
	char * tokens;
	int tokens_length;
} TokenList;

TokenList * alloc_token_list(char *);


#endif
