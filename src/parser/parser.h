#ifndef	PARSER_H
#define PARSER_H

#include<stdlib.h>
#include<stdint.h>
#include<stdio.h>
#include<iterator>

#include"lexer.h"
#include"TagMap.h"

class Parser {
private:
    Lexer lxr;
    TagMap map;
public:
    Parser(char * begin, char *, TagMap &map);
    
    template<typename ts_oiter,typename  off_oiter>
     void parse(ts_oiter &ts, off_oiter &os) {
        Tag tag;
        while(get_next_tag(& lxr, &tag) == 1)
        {
            //Key k;
            switch(tag.type)
            {
                case START_END_TAG: {
                    TagKey k(tag.begin + 1, tag.end_of_id);
                    token_type_t type = map[k];
                    *ts++ = type;
                    *os++ = tag.begin;
                    *ts++ = -type;
                    *os++ = tag.begin;
                    break;
                   }
                case START_TAG: {
                    TagKey k(tag.begin + 1, tag.end_of_id);
                    token_type_t type = map[k];
                    *ts++ = type;
                    *os++ = tag.begin;
                    break;
                 }
                case END_TAG:{
                    TagKey k(tag.begin + 2, tag.end_of_id);
                    token_type_t type = map[k];
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
#endif
