#ifndef TOKENSTREAM_H
#define TOKENSTREAM_H
#include"token_list.h"
#include<stdint.h>

typedef struct Tokenstream {
	Token * begin;
	Token * end;
	uint32_t size;
	struct Tokenstream * next;
} Tokenstream;

Token * get_token(Tokenstream *, Token *);
//void insert_token(Tokenstream *, Token);
Token * get_new_token_pointer(Tokenstream **, Token *);
void create_tokenstream(Tokenstream *,uint32_t);

#endif
