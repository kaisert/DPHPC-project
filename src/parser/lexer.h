#ifndef LEXER_H
#define LEXER_H

#include<stdlib.h>

typedef enum{
	START_TAG,
	END_TAG,
	START_END_TAG,
    XML_DECLARATION
} TagType;


struct Tag {
	char * begin;
	char * end_of_id;
	char * end;
	TagType type;
};
typedef struct Tag Tag;

typedef struct Lexer {
	char * begin;
	char * end;
} Lexer;

extern int get_next_tag(Lexer * const lxr, Tag * tag);


#endif
