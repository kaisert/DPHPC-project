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

#include<iostream>

template<typename iter>
class SurpressClosingVSizeDeflator {
public:
    SurpressClosingVSizeDeflator(int k_token_count, iter it)
    : out_iterator(it),
    current_cmpr_token_count(0),
    k_token_count(k_token_count),
    token_size(0),
    current_bit_off(15)
    {
        int i = k_token_count;
        while(i >>= 1) token_size++;
    }


    using Token_Type = std::allocator<token_type_t>;
    typedef Token_Type allocator_type;
    typedef typename Token_Type::value_type value_type;
    typedef typename Token_Type::reference reference;
    typedef typename Token_Type::const_reference const_reference;
    typedef typename Token_Type::difference_type difference_type;
    typedef typename Token_Type::size_type size_type;
    typedef std::output_iterator_tag iterator_category;

    inline SurpressClosingVSizeDeflator& operator++() {return *this; }
    inline SurpressClosingVSizeDeflator& operator++(int){return *this;}
    inline SurpressClosingVSizeDeflator& operator*() { return *this; }

    void flush()
    {
        set_bit(&current_bitmask, MAX_TOKEN_CMPR - current_cmpr_token_count);
        *out_iterator++ = current_bitmask;
        uint16_t new_push_back = 0;
        bool new_token_exists = write_token_to_uint16(0x000, &current_push_back,
                &new_push_back);
        compressed_tokens.push_back(current_push_back);
        if(new_token_exists) {
            compressed_tokens.push_back(new_push_back);
        }
        push_back_cmpr_tokens();
    }

    SurpressClosingVSizeDeflator& operator=(token_type_t t)
    {
        std::cerr << std::hex << t << "\n"; 
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
            uint16_t new_push_back = 0;
            if(write_token_to_uint16(t, &current_push_back, &new_push_back))
            {
               compressed_tokens.push_back(current_push_back);
               current_push_back = new_push_back;
            }
        }
        current_cmpr_token_count++;
        return *this;
    }
private:
    iter out_iterator;
    int current_cmpr_token_count, k_token_count, token_size, current_bit_off;
    uint16_t current_bitmask, current_push_back;
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

    inline bool write_token_to_uint16(uint16_t token, uint16_t *out1, uint16_t *out2)
    {
        if(current_bit_off - token_size < 0)
        {
            uint16_t mask2 = ~(0x8000 >> (15 - (token_size - current_bit_off)));
            uint16_t part1 = token >> (token_size - current_bit_off);
            uint16_t part2 = (token & mask2) << (15 - (token_size - current_bit_off));
            *out1 |= part1;
            *out2 = part2;
            current_bit_off = 15 - (token_size - current_bitmask);
            std::cerr << "blubb";
            return true;
        } else {
            current_bit_off -= token_size;
            *out1 |= token << (current_bit_off);
            std::cerr << "out: " << std::hex << "0x" << *out1 << "\n";
            return false;
        }
    }
};


#endif //DPHPC15_SURPRESSCLOSINGDEFLATOR_H
