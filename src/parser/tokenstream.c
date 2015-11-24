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

Tokenstream* tokenstream_get(Tokenstream* ts, int idx) {
    int count = 0;

    while((count != idx) && (ts != NULL)) {
        ts = ts->next;
        count++;
    }

    if(count != idx)
        return NULL;
    return ts;
}

Tokenstream* tokenstream_init(size_t buf_size) {
    Tokenstream* ts = (Tokenstream*) malloc(sizeof(Tokenstream));
    create_tokenstream(ts, buf_size);
    return ts;
}

Tokenstream* tokenstream_append(Tokenstream* ts) {
    while(ts->next != NULL) {
        ts = ts->next;
    }
    
    Tokenstream* ts_new = NULL;
    
    if(ts != NULL) {
        ts_new = (Tokenstream*) malloc(sizeof(Tokenstream));
        create_tokenstream(ts_new, ts->size);
    }

    return ts_new;
}

void tokenstream_rewind(Tokenstream* ts) {
    if(ts->end > ts->begin) {
        ts->end--;
    }
}
