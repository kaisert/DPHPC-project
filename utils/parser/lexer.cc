#include"lexer.h"

namespace Parser {
    Lexer::Lexer(char *first, char *last):
            first(first),
            last(last),
            null_tag_(Tag(Tag::NULL_TAG, NULL, NULL, NULL)){}

    Tag Lexer::get_next() {
        for (;;) {
            if (first != last) {
                Tag::Type tag_type;
                switch (*first) {
                    case '<': {
                        char *current_begin = first;
                        tag_type = lex_tag_type();
                        lex_name();
                        char *end_of_identifier = first;
                        lex_arguments();
                        if (*first == '/') {
                            tag_type = Tag::Type::START_END_TAG;
                            first++;
                        }
                        char *current_end = first;
                        return Tag(tag_type, current_begin, end_of_identifier, current_end);
                    }
                    case ' ':
                    case '\t':
                    case '\n':
                    case '\r':
                    case '>':
                        first++;
                        break;
                    default:
                        lex_text();
                        break;
                }
            } else {
                return null_tag_;
            }
        }
    }

    Tag::Type Lexer::lex_tag_type() {
        first++;
        Tag::Type type;
        if (*first == '/') {
            type = Tag::END_TAG;
            first++;
        } else {
            type = Tag::START_TAG;
        }
        first++;
        return type;
    }

    void Lexer::lex_name(){
        while(!is_separator(*first)){
            first++;
        }
    }

    void Lexer::lex_text() {
        while (*first != '<') {
            first++;
        }
    }

    void Lexer::lex_arguments() {
        while(*first != '>' && *first != '/'){
            first++;
        }
    }

    bool Lexer::is_separator(char c) {
        return c == 0x20 ||
               c == 0x9 ||
               c == 0xD ||
               c == 0xA;
    }
}

