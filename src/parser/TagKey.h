//
// Created by tobias on 12/10/15.
//

#ifndef DPHPC15_TAGKEY_H
#define DPHPC15_TAGKEY_H

#include<cstring>

class TagKey {
public:
    TagKey(char* begin, char* end, std::size_t hash_value)
            : begin(begin),
              end(end),
              hash_value(hash_value)
    {}
    TagKey(char* begin, char* end)
            : begin(begin),
              end(end)
    {
        std::size_t hash = 5381;
        for(char* current = begin; current != end; ++current) {
            hash = ((hash << 5) + hash) + *current;
        }
        hash_value = hash;
    }

    char *begin, *end;
    std::size_t hash_value;

    bool operator==(const TagKey &other) const {
        return hash_value == other.hash_value &&
        strings_are_equal(*this, other);
    }

private:
    bool strings_are_equal(const TagKey &a, const TagKey &b) const {
        std::size_t size_a = a.end - a.begin;
        std::size_t size_b = b.end - b.begin;
        int cmp = std::strncmp(a.begin, b.begin, std::min(size_a, size_b));
        return cmp == 0;
    }

};
#endif //DPHPC15_TAGKEY_H
