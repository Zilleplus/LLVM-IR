#pragma once
#include<string>

namespace infra{

enum class TokenType{
    eof,

    def,
    ext,

    identifier,
    number,

    plus,
    minus,
    smaller_then,
    greater_then,
    semicolon,

    left_paren,
    right_paren,

    if_,
    then,
    else_
};

std::string ToString(TokenType);

struct Token{
    TokenType type;
    int line_number;
    int position;
    std::string txt;
};

std::string ToString(const Token& t);

}