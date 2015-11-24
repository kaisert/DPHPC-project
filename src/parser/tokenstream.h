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

/*
 * returns the next token after current.
 * returns null when stream is finished.
 */
Token * ts_get_token(Tokenstream *ts, Token *current);

extern void create_tokenstream(Tokenstream *ts, uint32_t size);

extern Tokenstream* tokenstream_init(size_t buf_size);
extern Tokenstream* tokenstream_append(Tokenstream* ts);
extern void tokenstream_rewind(Tokenstream* ts);

/*
 * destroy all tokenstreams connected to ts
 */
extern void destroy_tokenstreams(Tokenstream *ts);
inline Token * get_new_token_pointer(Tokenstream ** ts)
{
    Tokenstream* current_ts = *ts;

	if(current_ts->end == current_ts->begin + current_ts->size)
	{
		Tokenstream * new_ts = (Tokenstream*) malloc(sizeof(Tokenstream));
		create_tokenstream(new_ts, (*ts)->size);
		new_ts->next = (*ts)->next;
		(*ts)->next = new_ts;
		*ts = new_ts;
		current_ts = new_ts;
	}
	Token* current = current_ts->end;
    current_ts->end++;
	return current;
}
#endif
