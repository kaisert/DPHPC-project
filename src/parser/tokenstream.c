#include"tokenstream.h"

void create_tokenstream(Tokenstream *ts, uint32_t size) {
	ts->begin = malloc(sizeof(Token) * size);
	if(ts->begin == NULL) 
	{
		printf("Error, when allocating new tokenstream\n");
		exit(0);
	}
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
	return current + 1;
}

extern Token* get_new_token_pointer(Tokenstream **ts);

/*
 * destroys all tokenstreams connected to ts
 */
void destroy_tokenstreams(Tokenstream *ts)
{
    while(ts != NULL)
    {
        free(ts->begin);
        Tokenstream * temp = ts;
        ts = ts->next;
        free(temp);
    }
}
