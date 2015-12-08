#ifndef DPHPC15_SURPRESSCLOSINGDEFLATOR_H
#define DPHPC15_SURPRESSCLOSINGDEFLATOR_H

#include <memory>
#include <vector>
#include <iterator>
#include "../parser/token_list.h"
#include "BitmaskUtils.h"
#include "SupressClosingVSizeInflator.h"

#include<iostream>

#ifndef NULL_TOKEN
#define NULL_TOKEN ((cmpr_token_t) 0x0)
#endif

template<typename bitmask_t, typename cmpr_token_t>
class SupressClosingVSizeDeflator: public std::iterator<std::output_iterator_tag, token_type_t> {
private:
    typedef typename std::vector<cmpr_token_t>::const_iterator cmpr_iterator;
    typedef SupressClosingVSizeInflator<
            cmpr_iterator,
            bitmask_t,
            cmpr_token_t> Inflator;
public:
    SupressClosingVSizeDeflator()
        :current_cmpr_token_count(0),
        token_size(0),
        current_bitmask_os(BIT_SIZE(bitmask_t) - 1),
        current_bitmask(0),
        remaining_token(0),
        remaining_token_os(BIT_SIZE(cmpr_token_t)),
        cmpr_t_cont_bin(std::back_inserter(cmpr_t_cont)),
        token_count(0)
    {
    }

    using Token_Type = std::allocator<token_type_t>;
    typedef Token_Type allocator_type;
    //typedef typename Token_Type::value_type typedef typename Token_Type::reference reference;
    typedef typename Token_Type::const_reference const_reference;
    typedef typename Token_Type::difference_type difference_type;
    typedef typename Token_Type::size_type size_type;
    typedef std::output_iterator_tag iterator_category;

    inline SupressClosingVSizeDeflator& operator++() {return *this; }
    inline SupressClosingVSizeDeflator& operator++(int){return *this;}
    inline SupressClosingVSizeDeflator& operator*() { return *this; }

    void init(int token_count) {
      int i = token_count + 1;
      do{token_size++;} while(i >>= 1);
    }
    
    int size() { return token_count;}

    void flush()
    {
        set_bit(&current_bitmask, BIT_SIZE(bitmask_t) -1 - current_cmpr_token_count);
        t_buf.push_back(NULL_TOKEN);
        push_back_tokens();
        *cmpr_t_cont_bin++ = remaining_token;
    }


    Inflator begin()
    {
        return Inflator(token_count, cmpr_t_cont.begin());
    }

    Inflator& end()
    {
        return end_inflator;
    }

   // SupressClosingVSizeDeflator& operator=(token_type_t t)
    void push_back(token_type_t t)
    {
        int pos = BIT_SIZE(bitmask_t) - 1 - current_cmpr_token_count;
        if(t < 0)
        {       
            unset_bit(&current_bitmask, pos);
        } else {
            set_bit(&current_bitmask, pos); 
            t_buf.push_back(t);
        }
        current_cmpr_token_count++;

        if(current_cmpr_token_count == BIT_SIZE(bitmask_t))
        {
            push_back_tokens();
            current_cmpr_token_count = 0;
            t_buf.erase(t_buf.begin(), t_buf.end());
            current_bitmask = 0;
        }
        token_count++;
    }

    void reserve(std::size_t n) {cmpr_t_cont.reserve(n);}
    //void push_back(token_type_t t){(*this) = t;}
private:

    inline void push_back_tokens()
    {
        push_back_one_token(current_bitmask,
                BIT_SIZE(bitmask_t),
                &remaining_token,
                &remaining_token_os,
                cmpr_t_cont_bin);
        for(auto it = t_buf.begin(); it != t_buf.end(); it++)
        {
           push_back_one_token(*it,
                   token_size,
                   &remaining_token,
                   &remaining_token_os,
                   cmpr_t_cont_bin);
        }
    }

    int current_cmpr_token_count; //number of token that are compressed but not
                                  //yet flushed
    int token_size; //size of the compressed tokens
    int16_t current_bitmask_os; //current bit offset of bitmask
    bitmask_t current_bitmask; //current bitmask for (opening/closing)
    cmpr_token_t remaining_token;
    int16_t remaining_token_os;
    int token_count;

    std::vector<token_type_t> t_buf;
    std::vector<cmpr_token_t> cmpr_t_cont;
    std::back_insert_iterator< std::vector<cmpr_token_t> > cmpr_t_cont_bin;
    Inflator end_inflator;
};

#endif //DPHPC15_SURPRESSCLOSINGDEFLATOR_H
