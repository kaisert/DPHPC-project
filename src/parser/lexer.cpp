#include"lexer.h" 

int is_separator(char c) {
	return c == 0x20 ||
		   c == 0x9 ||
		   c == 0xD ||
		   c == 0xA;
}
TagType lex_type(Lexer * const lxr)
{
	TagType type;
	lxr->begin++;
    switch(*(lxr->begin))
    {
        case '/':
            type = END_TAG;
            lxr->begin++;
            break;
        case '?':
            type = XML_DECLARATION;
            break;
        default:
            type = START_TAG;
            break;
    }
	return type;
}

void lex_name(Lexer * const lxr)
{
	while(! is_separator(*(lxr->begin)) && 
			*(lxr->begin) != '/' &&
			*(lxr->begin) != '>')
	{
		lxr->begin++;
	}
}

void lex_arguments(Lexer * const lxr)
{
	while(*(lxr->begin) != '>' && *(lxr->begin) != '/')
		lxr->begin++;
}

int get_next_tag(Lexer * lxr, Tag * tag)
{
	if(lxr == NULL)
		return -1;
	if(tag == NULL)
		return -2;
	
	for(;;)
	{
		if(lxr->begin != lxr->end)
		{
			switch(*(lxr->begin))
			{
				case '<':
					tag->begin = lxr->begin;
					tag->type = lex_type(lxr);
                    if(tag->type == XML_DECLARATION)
                        break;

					lex_name(lxr);
					tag->end_of_id = lxr->begin;
					lex_arguments(lxr);
					if(*(lxr->begin) == '/')
					{
						tag->type = START_END_TAG;
						lxr->begin++;
					}
					lxr->begin++;
					tag->end = lxr->begin;
					return 1;
				case ' ':
				case '\t':
				case '\n':
				case '\r':
				case '>':
				default:
					lxr->begin++;
					break;
			}
		}
		else
		{
			return 0;
		}
	}
}

