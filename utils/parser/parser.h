#ifndef PARSER_PARSER_H
#define PARSER_PARSER_H

#include <fstream>
#include <map>
#include <string.h>
#include <utility>
#include <algorithm>
#include <cstdint>

#include "PToken.h"
#include "Tag.h"
#include "lexer.h"

namespace Parser {
    class Parser {
    private:
        class compare {
        public:
            bool operator()(const std::pair<const char *, const char *> a, const std::pair<const char *, const char *> b);
        };

    public:
        class TokenIterator : public std::iterator<std::input_iterator_tag, PToken> {
        public:
            TokenIterator();
            TokenIterator(Parser *parser);
            TokenIterator(const TokenIterator &tit);

            TokenIterator &operator++();
            TokenIterator operator++(int);
            bool operator==(const TokenIterator &tit);
            bool operator!=(const TokenIterator &tit);
            PToken &operator*();

        private:
            PToken current_;
            Parser *parser_;
        };

    public:
        Parser(char * first, char * last);
        TokenIterator first();
        TokenIterator last();

        /*
         * returns the next Token
         */
        PToken get_token();
    private:
        Lexer lxr_;
        std::map<std::pair<const char *,const char *>, uint32_t, compare> token_map_;

        PToken remaining_end_token_;
        bool has_end_token_;

        /*
         * creates a token from a tag
         */
        PToken create_token(Tag tag);

        /*
         * reads in the tag name to token type map
         */
        bool read_token_map(std::string &path);

    };
}

#endif