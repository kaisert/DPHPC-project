#ifndef	PARSER_H
#define PARSER_H

#include "TagMap.h"
#include "lexer.h"

class Parser {
private:
    Lexer lxr;
    TagMap& map;
public:
    Parser(char * begin, char *, TagMap &map);
    
    template<typename ts_oiter,typename  off_oiter>
     void parse(ts_oiter &ts, off_oiter &os) {
        Tag tag;
        while(get_next_tag(& lxr, &tag) == 1)
        {
            switch(tag.type)
            {
                case START_END_TAG: {
                    TagKey k(tag.begin + 1, tag.end_of_id);
                    token_type_t type = map.get_value(k);

                    *ts++ = type;
                    *os++ = tag.begin;
                    *ts++ = -type;
                    *os++ = tag.begin;
                    break;
                   }
                case START_TAG: {
                    std::ostringstream ss;
                    TagKey k(tag.begin + 1, tag.end_of_id);
                    if(*(tag.begin + 1) == 'N')
//                    if(std::strncmp(tag.begin + 1, "N", 3) == 0)
                        for(auto c = tag.begin + 1; c != tag.end_of_id; ++c)
                            ss << *c;
                    token_type_t type = map.get_value(k);
                    ss << " type: " << type;
                    ss << std::endl;
                    //ss << "<";
                    //ss << type;
                    //ss << "> ";
                    std::cout << ss.str();
                    *ts++ = type;
                    *os++ = tag.begin;
                    break;
                 }
                case END_TAG:{
                    //std::ostringstream ss;
                    TagKey k(tag.begin + 2, tag.end_of_id);
                    token_type_t type = map.get_value(k);
                    //ss << "</";
                    //ss << type;
                    //ss << "> ";
//                    std::cout << ss.str();
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
