#ifndef PARSER_LEXER_H
#define PARSER_LEXER_H

#include<string>
#include<functional>
#include<iterator>

#include"Tag.h"

namespace Parser {

    class Lexer {
    public:
        Lexer(char *first, char *last);

        /*
         * Lexes and returns the next Tag
         */
        Tag get_next();
    private:
        const Tag null_tag_ = Tag(Tag::NULL_TAG, NULL, NULL, NULL);

        char * first;
        char * last;

        Tag::Type lex_tag_type();

        /*
         * lexes a tag name
         */
        void lex_name();

        /*
         * lexes tag arguments
         */
        void lex_arguments();

        /*
         * lexes xml text
         */
        void lex_text();

        /*
         * checks whether c is an xml separator
         */
        bool is_separator(char c);
    };
}

#endif

