//
// Created by Stefan Dietiker on 29/11/15.
//

#ifndef DPHPC15_SURPRESSCLOSINGDEFLATOR_H
#define DPHPC15_SURPRESSCLOSINGDEFLATOR_H

#define MAX_TOKEN_CMPR 15
#define NULL_TOKEN 0x0000

#include <memory>
#include <vector>
#include "../parser/token_list.h"

template<typename iter>
class SurpressClosingDeflator {
public:
    SurpressClosingDeflator(iter it)
    : out_iterator(it),
    current_cmpr_token_count(0)
    {}

    using Token_Type = std::allocator<token_type_t>;
    typedef Token_Type allocator_type;
    typedef typename Token_Type::value_type value_type;
    typedef typename Token_Type::reference reference;
    typedef typename Token_Type::const_reference const_reference;
    typedef typename Token_Type::difference_type difference_type;
    typedef typename Token_Type::size_type size_type;
    typedef std::output_iterator_tag iterator_category;

    inline SurpressClosingDeflator& operator++() {return *this; };
    inline SurpressClosingDeflator& operator++(int){return *this;};
    inline SurpressClosingDeflator& operator*() { return *this; };

    void flush()
    {
        int size = compressed_tokens.size();
        set_bit(&current_bitmask, size);
        *out_iterator++ = current_bitmask;
        push_back_cmpr_tokens();
        *out_iterator++ = NULL_TOKEN;
    }

    SurpressClosingDeflator& operator=(token_type_t t)
    {
        if(current_cmpr_token_count == MAX_TOKEN_CMPR)
        {
            *out_iterator++ = current_bitmask;
            push_back_cmpr_tokens();
            current_cmpr_token_count = 0;
        }
        if(t < 0)
        {       
            unset_bit(&current_bitmask, MAX_TOKEN_CMPR - current_cmpr_token_count);
        } else {
            set_bit(&current_bitmask, MAX_TOKEN_CMPR - current_cmpr_token_count);
            compressed_tokens.push_back(t);
        }
        current_cmpr_token_count++;
        return *this;
    }


private:
    iter out_iterator;
    int current_cmpr_token_count;
    uint16_t current_bitmask;
    std::vector<token_type_t> compressed_tokens;

    void push_back_cmpr_tokens()
    {
        for(std::vector<token_type_t>::iterator curr = compressed_tokens.begin();
                curr != compressed_tokens.end(); ++curr) 
        {
            *out_iterator++ = *curr;
        }
        compressed_tokens.erase(compressed_tokens.begin(), compressed_tokens.end());
    }

    inline void set_bit(uint16_t *word, uint8_t bit)
    {
        uint16_t mask = 0x0001 << bit;
        *word |= mask;
    }

    inline void unset_bit(uint16_t *word, uint8_t bit)
    {
        uint16_t mask = ~ (0x0001 << bit);
        *word &= mask;
    }
};


#endif //DPHPC15_SURPRESSCLOSINGDEFLATOR_H
