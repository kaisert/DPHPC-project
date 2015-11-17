#ifndef PARSER_TAG_H
#define PARSER_TAG_H
#include<iterator>

namespace Parser {

    struct Tag {
    public:
        enum Type {
            START_TAG, END_TAG, START_END_TAG, TEXT, NULL_TAG
        };

    public:
        Tag(const Type &type, const char * begin, const char * end_of_identifier, const char * end) :
                type(type),
                begin(begin),
                end_of_identifier(end_of_identifier),
                end(end) { }
        const Type type;
        const char * begin;
        const char * end_of_identifier;
        const char * end;
    };
}

#endif