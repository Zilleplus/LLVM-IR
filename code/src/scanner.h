#pragma once

#include <string>
#include <token.h>
#include <vector>

namespace infra
{
    class Scanner
    {
        const std::string &source;
        std::vector<Token> tokens;

        int start;
        int current;
        int line;

        char Advance();
        char Peek() const;
        char PeekNext() const;

        bool IsAtEnd() const;

        void AddToken(TokenType t);

        void Number();

        void Indentifer();

        std::string Text() const;

        bool Match(char c);

    public:
        explicit Scanner(const std::string &source) : source(source),
                                                      tokens({}),
                                                      start(0),
                                                      current(0),
                                                      line(0) {}
        const std::vector<Token> &ScanTokens();
    };
}