#ifndef PARSER_P_TOKEN_H
#define PARSER_P_TOKEN_H

#include<cstdint>

namespace Parser {

    struct PToken {
    public:
        PToken(const uint32_t ttype, const char *first, const char *last)
                : ttype(ttype),
                  first(first),
                  last(last) { }

        const uint32_t ttype;
        const char *first;
        const char *last;

        bool operator==(const PToken &other) {
            return ttype == other.ttype && first == other.first && last == other.last;
        }

        const PToken &operator=(const PToken &other) {
            return other;
        }
    };
}

#endif