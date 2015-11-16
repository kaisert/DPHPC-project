//
// Created by tobias on 11/16/15.
//

#include "TokenIterator.h"

namespace Parser {

    TokenIterator::TokenIterator(std::function<PToken(void)> get_token) :
    current_(get_token())
    {}

    TokenIterator::TokenIterator(const TokenIterator &tit) :
            current_(tit.current_),
            get_token_(tit.get_token_) {
    }

    TokenIterator &TokenIterator::operator++() {
        current_ = get_token_();
        return *this;
    }

    TokenIterator TokenIterator::operator++(int) {
        TokenIterator tmp(*this);
        operator++();
        return tmp;
    }

    bool TokenIterator::operator==(const TokenIterator &tit) {
        return current_ == rhs.current_;
    }

    bool TokenIterator::operator!=(const TokenIterator &tit) {
        return !(current_ == rhs.current_);
    }

    PToken &TokenIterator::operator*() { return current_; }
}