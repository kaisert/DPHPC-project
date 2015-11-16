#include<iterator>
#include "PToken.h"
#include <functional>

namespace Parser {

    class TokenIterator : public std::iterator<std::input_iterator_tag, PToken> {
    public:
        TokenIterator(std::function<PToken(void)> get_token);
        TokenIterator(const TokenIterator &tit);

        TokenIterator &operator++();
        TokenIterator operator++(int);
        bool operator==(const TokenIterator &tit);
        bool operator!=(const TokenIterator &tit);
        PToken &operator*();

    private:
        PToken current_;
        std::function<PToken(void)> get_token_;

    };
}