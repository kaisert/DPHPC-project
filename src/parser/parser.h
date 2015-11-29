#ifndef	PARSER_H
#define PARSER_H

#include<stdlib.h>
#include<stdint.h>
#include<stdio.h>
#include<iterator>

#include"lexer.h"
#include"map.h"
#include"token_list.h"

class Parser {
private:
    Lexer lxr;
    Map *map;
    Token remaining_token;
    bool remaining_token_exists;
public:
    Parser(char * begin, char *, Map *map);
    
    template<typename ts_oiter,typename  off_oiter>
     void parse(ts_oiter &ts, off_oiter &os) {
        Tag tag;
        while(get_next_tag(& lxr, &tag) == 1)
        {
            Key k;
            switch(tag.type)
            {
                case START_END_TAG: {
                    k.begin = tag.begin + 1;
                    k.end = tag.end_of_id;
                    token_type_t type = get_value(&k, map);
                    *ts++ = type;
                    *os++ = tag.begin;
                    *ts++ = -type;
                    *os++ = tag.begin;
                    break;
                   }
                case START_TAG: {
                    k.begin = tag.begin + 1;
                    k.end = tag.end_of_id;
                    token_type_t type = get_value(&k, map);
                    *ts++ = type;
                    *os++ = tag.begin;
                    break;
                 }
                case END_TAG:{
                    k.begin = tag.begin + 2;
                    k.end = tag.end_of_id;
                    token_type_t type = (get_value(&k, map));
                    *ts++ = -type;
                    *os++ = tag.begin;
                    break;
                }
                case XML_DECLARATION: {
                    break;
                 }
            }
        }
    }
    
};
/*
typedef struct Parser {
	Lexer lxr;
	Map map;
	Token remaining_token;
	uint16_t remaining_token_exists;
} Parser;

extern Parser * alloc_parser(char *, char *);
extern int get_next_token(Parser *, Token *);
extern void init_parser(Parser *, Map*);
*/
#endif
