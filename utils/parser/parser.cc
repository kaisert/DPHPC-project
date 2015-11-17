#include"parser.h"

namespace Parser {

    Parser::Parser(char *first, char *last)
            : lxr_(Lexer(first, last)),
              remaining_end_token_(PToken(0, NULL, NULL)){
        //TODO
        std::string s = "asfd";
        read_token_map(s);
    }

    Parser::TokenIterator Parser::first() { return TokenIterator(this); }

    Parser::TokenIterator Parser::last() { return TokenIterator(); }

    PToken Parser::get_token() {
        if (has_end_token_) {
            has_end_token_ = false;
            return remaining_end_token_;
        } else {
            Tag tag = lxr_.get_next();
            return create_token(tag);
        }
    }

    PToken Parser::create_token(Tag tag) {
        uint32_t ttype = 0;
        std::pair<const char *, const char *> id = std::make_pair(tag.begin, tag.end_of_identifier);
        switch (tag.type) {
            case Tag::Type::START_END_TAG:
                has_end_token_ = true;
                remaining_end_token_ = PToken(~(token_map_[id]), tag.begin, tag.end);
            case Tag::Type::START_TAG:
                ttype = token_map_[id];
                break;
            case Tag::Type::END_TAG:
                ttype = ~(token_map_[id]);
                break;
            case Tag::Type::NULL_TAG:
                ttype = 0;
                break;
        }
        return PToken(ttype, tag.begin, tag.end);
    }

    bool Parser::read_token_map(std::string &path) {
        std::ifstream infile(path);
        infile.seekg(0, std::ifstream::end);
        long size = infile.tellg();
        char *tokens = new char[size];
        infile.seekg(0);
        infile.read(tokens, size);

        char *start_token = tokens, *end_token;
        uint32_t i = 1;
        for (char *p = tokens; p < tokens + size; ++p) {
            if (*p == '\n') {
                end_token = p;
                token_map_[std::make_pair(start_token, end_token)] = i++;
            }
        }
        return !token_map_.empty();
    }

    bool Parser::compare::operator()(const std::pair<const char *,const char *> a, const std::pair<const char *,const char *> b) {
        uint64_t size_a = a.second - a.first;
        uint64_t size_b = b.second - b.first;
        int cmp = strncmp(a.first, b.first, std::min(size_a, size_b));
        if(size_a == size_b)
            return cmp < 0;
        else if(cmp == 0)
            return size_a < size_b;
        else
            return cmp < 0;
    }

    Parser::TokenIterator::TokenIterator():
            current_(PToken(0,0,0)),
            parser_(NULL)
    {}
    Parser::TokenIterator::TokenIterator(Parser *parser) :
            parser_(parser),
            current_(parser->get_token())
    {}

    Parser::TokenIterator::TokenIterator(const TokenIterator &tit) :
            parser_(tit.parser_),
            current_(tit.current_)
    {}

    Parser::TokenIterator &Parser::TokenIterator::operator++() {
        if(parser_ != NULL)
            current_ = parser_->get_token();
        return *this;
    }

    Parser::TokenIterator Parser::TokenIterator::operator++(int) {
        TokenIterator tmp(*this);
        operator++();
        return tmp;
    }

    bool Parser::TokenIterator::operator==(const TokenIterator &tit) {
        return current_ == tit.current_;
    }

    bool Parser::TokenIterator::operator!=(const TokenIterator &tit) {
        return !(current_ == tit.current_);
    }

    PToken &Parser::TokenIterator::operator*() { return current_; }
}