//
// Created by Stefan Dietiker on 29/11/15.
//

#ifndef DPHPC15_SURPRESSCLOSINGDEFLATOR_H
#define DPHPC15_SURPRESSCLOSINGDEFLATOR_H


#include <memory>
#include <vector>
#include "../parser/token_list.h"

class SurpressClosingDeflator {
public:
    SurpressClosingDeflator();

    using Token_Type = std::allocator<token_type_t>;
    typedef Token_Type allocator_type;
    typedef typename Token_Type::value_type value_type;
    typedef typename Token_Type::reference reference;
    typedef typename Token_Type::const_reference const_reference;
    typedef typename Token_Type::difference_type difference_type;
    typedef typename Token_Type::size_type size_type;
    typedef std::output_iterator_tag iterator_category;

    inline SurpressClosingDeflator& operator++() {return *this; };
    inline SurpressClosingDeflator& operator*() { return *this; };

    SurpressClosingDeflator& operator=(Token_Type t);
};


#endif //DPHPC15_SURPRESSCLOSINGDEFLATOR_H
