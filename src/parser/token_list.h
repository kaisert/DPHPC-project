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

void init_token_list(TokenList *token_list, const char * path);
void print_token(Token * token);
void destroy_token_list(TokenList * list);


#endif
