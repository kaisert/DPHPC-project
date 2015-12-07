#ifndef SUPRESS_CLOSING_V_SIZE_INFLATOR_H
#define SUPRESS_CLOSING_V_SIZE_INFLATOR_H

#include<cstdint>
#include<iterator>
#include"../parser/token_list.h"
#include"BitmaskUtils.h"

#ifndef BIT_SIZE
#define BIT_SIZE(x) (sizeof(x) * 8)
#endif

#ifndef NULL_TOKEN
#define NULL_TOKEN 0
#endif

#ifndef CLOSING_TAG
#define CLOSING_TAG -1
#endif

template<typename cmpr_token_iter, 
    typename bitmask_t, 
    typename cmpr_token_t>
class SupressClosingVSizeInflator {
public:
    SupressClosingVSizeInflator(int k_token_count, cmpr_token_iter ct_it)
        :ct_it(ct_it),
        cmpr_token_size(0),
        remaining(0),
        remaining_bit_count(0),
        bitmask(0),
        bitmask_remaining(0),
        current_token(-1)
    {
        int i = k_token_count + 1;
        do{cmpr_token_size++;}while(i >>= 1);
        (*this)++;
    }

    friend bool operator==(const SupressClosingVSizeInflator& a, 
            const SupressClosingVSizeInflator& b) 
    {
        return a.current_token == b.current_token;
    }

    friend bool operator!=(const SupressClosingVSizeInflator& a, 
            const SupressClosingVSizeInflator& b)
    {
        return !operator==(a,b); 
    }
    
    inline token_type_t operator*(){return current_token;}
    inline token_type_t * operator->(){return &current_token;}

    SupressClosingVSizeInflator& operator++(int)
    {
        if(current_token == NULL_TOKEN)
            return *this;

        if(bitmask_remaining == 0)
        {
            extract_bitmask(
                    ct_it,
                    &remaining,
                    &remaining_bit_count,
                    &bitmask);
            bitmask_remaining = BIT_SIZE(bitmask_t);
        }
        
        token_type_t token;
        if((bitmask >> (bitmask_remaining - 1)) & 1)
        {
            token = extract_token(
                    ct_it, 
                    &remaining,
                    &remaining_bit_count,
                    cmpr_token_size
                    );
        } 
        else 
        {
            token = CLOSING_TAG;
        }
        bitmask_remaining--;
        

        current_token = token;
        return *this;
    }

private:
    cmpr_token_iter ct_it;
    int cmpr_token_size;
    cmpr_token_t remaining;
    int16_t remaining_bit_count;
    bitmask_t bitmask;
    int16_t bitmask_remaining;
    token_type_t current_token;
};

#endif
