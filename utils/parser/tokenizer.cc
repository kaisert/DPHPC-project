//
// Created by tobias on 11/9/15.
//
#include <iostream>
#include <vector>
#include "tokenizer.h"

void Tokenizer::start_tag(start_tag_event fn) {
    start_tag_fn = fn;
}

void Tokenizer::end_tag(end_tag_event fn) {
    end_tag_fn = fn;
}

void Tokenizer::text_tag(text_event fn) {
    text_fn = fn;
}

std::string Tokenizer::parse_nametag(std::istream & stream) {
    bool tag_finished = false;
    std::string nametag = "";
    while(!tag_finished){
        char current = stream.peek();
        switch(current){
            case '=':
            case ' ':
            case '"':
                stream.get();
            case '>':
                tag_finished = true;
                break;
            default:
                stream.get();
                nametag += current;
                break;
        }
    }
    return nametag;
}

Tokenizer::Attributes Tokenizer::parse_attributes(std::istream &stream) {
    bool attributes_finished = stream.peek() == '>';
    Attributes att;
    while(!attributes_finished){
        attribute a;
        a.id = parse_nametag(stream);
        stream.get();
        a.value = parse_nametag(stream);
        att.push_back(a);
        attributes_finished = stream.get() == '>';
    }
    return att;
}

void Tokenizer::parse(std::istream &stream) {
    char current;
    while(stream.get(current)) {
        if (current == std::char_traits<char>::eof()) {
            break;
        }

        switch (current) {
            case '<':
            //std::cout << "enter < case\n" ;
                if (stream.peek() == '/') {
                    stream.get();
                    std::string nametag = parse_nametag(stream);
                    end_tag_fn(nametag);
                } else {
                    std::string nametag = parse_nametag(stream);
                    Attributes att = parse_attributes(stream);
                    start_tag_fn(nametag, att);
                }
               // std::cout << "leave < case\n";
                break;
            case ' ':
            case '\t':
            case '\n':
            case '\r':
            case '>':
                break;
            default:
                std::string text = parse_nametag(stream);
                text_fn(text);
                break;
        }
    }
}
