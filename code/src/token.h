#pragma once
#include <string>

namespace infra
{

    enum class TokenType
    {
        eof,

        def,
        ext,

        identifier,
        number,

        plus,
        minus,
        asterisk,
        smaller_then,
        greater_then,
        semicolon,
        comma,

        left_paren,
        right_paren,

        if_,
        then,
        else_
    };

    std::string ToString(TokenType);

    struct Token
    {
        TokenType type;
        int line_number;
        int position;
        std::string txt;

        bool operator==(const Token &other) const
        {
            bool type_eq = type == other.type;
            bool line_number_eq = line_number == other.line_number;
            bool position_eq = position == other.position;
            bool text_eq = txt == other.txt;

            return type_eq && line_number_eq && position_eq && text_eq;
        }
    };

    std::string ToString(const Token &t);

}