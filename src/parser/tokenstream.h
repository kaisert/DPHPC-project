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

Token * ts_get_token(Tokenstream *, Token *);

void create_tokenstream(Tokenstream *,uint32_t);

inline Token * get_new_token_pointer(Tokenstream ** ts)
{
    Tokenstream* new_ts = *ts;

	if(new_ts->end == new_ts->begin + new_ts->size)
	{
		Tokenstream * new_ts = (Tokenstream*) malloc(sizeof(Tokenstream));
		create_tokenstream(new_ts, (*ts)->size);
		new_ts->next = (*ts)->next;
		(*ts)->next = new_ts;
		*ts = new_ts;
	}
	Token* current = new_ts->end;
    new_ts->end++;
	return current;
}
#endif
