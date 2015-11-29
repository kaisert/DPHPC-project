#include"parser.h"

void init_lexer(Lexer &lxr, char * begin, char *end)
{
	lxr.begin = begin;
	lxr.end = end;
}

Parser::Parser(char * begin, char * end, Map * map)
    :map(map)
{
    init_lexer(lxr, begin, end);
}

/*Parser * alloc_parser(char * begin, char * end)
{
	Parser * prs = static_cast<Parser*>(malloc(sizeof(Parser)));
	init_lexer(prs, begin, end);
	prs->remaining_token_exists = 0;
	return prs;
}*/

/*void destroy_parser(Parser * prs)
{
	free(prs);
}*/

/*int get_next_token(Parser * prs, Token * token)
{
	if(prs == NULL)
		return -1;
	if(token == NULL)
		return -2;
	
	if(prs->remaining_token_exists == 1)
	{
		token->type = prs->remaining_token.type;
		token->begin = prs->remaining_token.begin;
		token->end = prs->remaining_token.end;
		prs->remaining_token_exists = 0;
		return 1;
	}

	Tag tag;
	if(get_next_tag(& (prs->lxr), &tag) == 0)
	{
		return 0;
	}

	token->begin = tag.begin;
	token->end = tag.end;
	
	Key k;
	switch(tag.type)
	{
		case START_END_TAG: {
			k.begin = tag.begin + 1;
			k.end = tag.end_of_id;
			uint32_t type = get_value(&k, &(prs->map));
			prs->remaining_token.type = ~ type;
			prs->remaining_token.begin = tag.begin;
			prs->remaining_token.end = tag.end;
			prs->remaining_token_exists = 1;
            }
		case START_TAG: {
			k.begin = tag.begin + 1;
			k.end = tag.end_of_id;
			token->type = get_value(&k, &(prs->map));
			break;
                            }
		case END_TAG:{
			k.begin = tag.begin + 2;
			k.end = tag.end_of_id;
			token->type = ~ (get_value(&k, &(prs->map)));
			break;
                     }
        case XML_DECLARATION: {
            return -1;
                              }
	}
	return 1;
}*/
