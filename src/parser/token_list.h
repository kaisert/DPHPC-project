#ifndef TOKEN_LIST_H
#define TOKEN_LIST_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef int16_t token_type_t;

typedef struct Token {
	token_type_t type;
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
