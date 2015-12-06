#ifndef BITMASKUTILS_H
#define BITMASKUTILS_H
#include<vector>
#include<iostream>
#include<cstdint>

#include<typeinfo>

#include"../parser/token_list.h"

#ifndef BIT_SIZE
#define BIT_SIZE(x) ((int16_t) (sizeof(x) * 8))
#endif

template<typename type>
inline void set_bit(type *word, int bit)
{
    type mask = 1;
    mask <<= bit;
    *word |= mask;
}

template<typename type>
inline void unset_bit(type *word, int bit)
{
    type mask = 1;
    mask <<= bit;
    mask = ~mask;
    *word &= mask;
}

template<typename src_t, typename trg_t>
inline void append_bits(src_t src,
        int16_t src_begin,
        int16_t src_end,
        trg_t *trg,
        int16_t trg_offset)
{
    int16_t size = src_end - src_begin;
    trg_t mask = (trg_t) -1;
    if(size >= BIT_SIZE(trg_t))
            mask = 0;
    else
        mask <<= (int) size;
    mask = ~mask;
    src >>= src_begin;
    src &= mask;
    trg_t src_ex = src;
    *trg |= src_ex << (trg_offset - size);
}

template<typename src_t, typename trg_t, typename iter>
inline void push_back_one_token(src_t src,
        int16_t src_size,
        trg_t *trg,
        int16_t *trg_offset,
        iter out_iterator)
{
    int16_t src_end = src_size;
    int16_t src_begin = src_end - *trg_offset;
    src_begin = src_begin < 0 ? 0 : src_begin;
    while(src_end != 0)
    {
        int16_t written_size = src_end - src_begin;
        append_bits(src, 
                src_begin, 
                src_end, 
                trg,
                *trg_offset);
        src_end = src_begin;
        src_begin -= BIT_SIZE(trg_t);
        src_begin = src_begin < 0 ? 0 : src_begin;
        *trg_offset -= written_size;
        if(*trg_offset <= 0)
        {
            *out_iterator++ = *trg;
            *trg = 0;
            *trg_offset = BIT_SIZE(trg_t);
        }
    }
}

template<typename cmpr_token_t, typename bitmask_t, typename iter>
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

template<typename cmpr_token_t, typename bitmask_t, typename in_iter>
inline void extract_bitmask(
        in_iter &it,
        cmpr_token_t *remaining,
        int16_t *remaining_bit_count,
        bitmask_t *bitmask)
{
    bitmask_t remaining_ex = *remaining;
    *bitmask = remaining_ex << 
        (BIT_SIZE(bitmask_t) - BIT_SIZE(cmpr_token_t));
    int16_t bits_read = *remaining_bit_count;
    while(bits_read < BIT_SIZE(bitmask_t))
    {
        cmpr_token_t cmpr_token = *it++;
        int16_t begin = BIT_SIZE(cmpr_token_t) - 
            (BIT_SIZE(bitmask_t) - bits_read);
        begin = begin < 0 ? 0 : begin;
        append_bits(cmpr_token,
                begin,
                BIT_SIZE(cmpr_token_t),
                bitmask,
                BIT_SIZE(bitmask_t) - bits_read);
        bits_read += BIT_SIZE(cmpr_token_t);
        *remaining = begin == 0 ? 0 : cmpr_token << 
            (BIT_SIZE(cmpr_token_t) - begin);
        *remaining_bit_count = begin;
    }
}

template<typename cmpr_token_t, typename in_iter>
inline token_type_t extract_token(
        in_iter &it,
        cmpr_token_t *remaining,
        int16_t *remaining_bit_count,
        int16_t cmpr_token_size)
{
   cmpr_token_t token;
   cmpr_token_t mask = -1;
   mask = (cmpr_token_size - *remaining_bit_count) >= BIT_SIZE(cmpr_token_t) ?
       0 : mask << (cmpr_token_size - *remaining_bit_count);
   mask = ~mask;
   if(*remaining_bit_count >= cmpr_token_size)
   {
        token = (*remaining >> (BIT_SIZE(cmpr_token_t) - cmpr_token_size));
        *remaining = cmpr_token_size >= BIT_SIZE(cmpr_token_t) ? 0 : 
            *remaining << cmpr_token_size;
        *remaining_bit_count -= cmpr_token_size;
   } else {
        token = (*remaining >> (BIT_SIZE(cmpr_token_t) - cmpr_token_size));
        *remaining = *it++;
        token |= (*remaining >> (BIT_SIZE(cmpr_token_t) - 
                    (cmpr_token_size - *remaining_bit_count))) & mask;
        *remaining <<= cmpr_token_size - *remaining_bit_count;
        *remaining_bit_count = BIT_SIZE(cmpr_token_t) - 
            (cmpr_token_size - *remaining_bit_count);
   }
   return token;
}
#endif
