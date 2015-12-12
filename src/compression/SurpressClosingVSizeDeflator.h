#ifndef DPHPC15_SURPRESSCLOSINGDEFLATOR_H
#define DPHPC15_SURPRESSCLOSINGDEFLATOR_H

#include <memory>
#include <vector>
#include "BitmaskUtils.h"

#include<iostream>

#define NULL_TOKEN ((cmpr_token_t) 0x0)

template<typename iter, typename bitmask_t, typename cmpr_token_t>
class SurpressClosingVSizeDeflator {
public:
    SurpressClosingVSizeDeflator(int k_token_count, iter &it)
        :current_cmpr_token_count(0),
        token_size(0),
        current_bitmask_os(BIT_SIZE(bitmask_t) - 1),
        current_bitmask(0),
        remaining_token(0),
        remaining_token_os(BIT_SIZE(cmpr_token_t)),
        out_iterator(it)
    {
        int i = k_token_count + 1;
        do{token_size++;} while(i >>= 1);
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
        set_bit(&current_bitmask, BIT_SIZE(bitmask_t) -1 - current_cmpr_token_count);
        compressed_tokens.push_back(NULL_TOKEN);
        push_back_tokens(current_bitmask,
                compressed_tokens,
                out_iterator,
                token_size,
                &remaining_token,
                &remaining_token_os);
        *out_iterator++ = remaining_token;
    }

    SurpressClosingVSizeDeflator& operator=(token_type_t t)
    {
        std::cerr << "0x" << std::hex << unsigned(t) << "\n";
        int pos = BIT_SIZE(bitmask_t) - 1 - current_cmpr_token_count;
        if(t < 0)
        {       
            unset_bit(&current_bitmask, pos);
        } else {
            set_bit(&current_bitmask, pos); 
            compressed_tokens.push_back(t);
        }
        current_cmpr_token_count++;

        if(current_cmpr_token_count == BIT_SIZE(bitmask_t))
        {
            push_back_tokens(current_bitmask, 
                    compressed_tokens,
                    out_iterator,
                    token_size,
                    &remaining_token,
                    &remaining_token_os);
            current_cmpr_token_count = 0;
            compressed_tokens.erase(compressed_tokens.begin(), 
                    compressed_tokens.end());
            current_bitmask = 0;
        }
        return *this;
    }

private:
    inline void push_back_tokens(bitmask_t bitmask, 
        std::vector<token_type_t> &tokens,
        iter &out_iterator, 
        int16_t cmpr_token_size,
        cmpr_token_t *remaining_token,
        int16_t *remaining_token_os)
    {
        push_back_one_token(bitmask,
                BIT_SIZE(bitmask_t),
                remaining_token,
                remaining_token_os,
                out_iterator);
        for(auto it = tokens.begin(); it != tokens.end(); it++)
        {
           push_back_one_token(*it,
                   cmpr_token_size,
                   remaining_token,
                   remaining_token_os,
                   out_iterator);
        }
    }
    int current_cmpr_token_count; //number of token that are compressed but not
                                  //yet flushed
    int token_size; //size of the compressed tokens
    int16_t current_bitmask_os; //current bit offset of bitmask
    bitmask_t current_bitmask; //current bitmask for (opening/closing)
    cmpr_token_t remaining_token;
    int16_t remaining_token_os;


    iter out_iterator;
    std::vector<token_type_t> compressed_tokens;
};

#endif //DPHPC15_SURPRESSCLOSINGDEFLATOR_H
