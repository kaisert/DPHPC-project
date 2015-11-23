#include"tokenstream.h"

void create_tokenstream(Tokenstream *ts, uint32_t size) {
	ts->begin = malloc(sizeof(Token) * size);
	ts->end = ts->begin;
	ts->size = size;
	ts->next = NULL;
}

/*int insert_token(Tokenstream * ts, Token t) 
{
	if(ts->end == ts->begin + ts->size)
	{
		return 0;
	}
	ts->end = t;
	ts->end++;
	return 1;
}*/

Token * ts_get_token(Tokenstream * ts, Token * current)
{
	if(current == NULL)
	{
		return ts->begin;
	}
	if(current + 1 == ts->end)
	{
		return NULL;
	}
	return current++;
}

extern Token* get_new_token_pointer(Tokenstream **);
