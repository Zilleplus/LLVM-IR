#include <LLVM_IR/scanner.h>
#include <ctype.h>
#include <unordered_map>

namespace infra
{
    static std::unordered_map<std::string, TokenType> keywords{
        {ToString(TokenType::def), TokenType::def},
        {ToString(TokenType::if_), TokenType::if_},
        {ToString(TokenType::then), TokenType::then},
        {ToString(TokenType::else_), TokenType::else_},
        {ToString(TokenType::ext), TokenType::ext}};

    bool IsAlpha(char c)
    {
        return std::isalpha(c) != 0;
    }

    bool IsDigit(char c)
    {
        return std::isdigit(c) != 0;
    }

    bool IsAlphaNumeric(char c)
    {
        return IsAlpha(c) || IsDigit(c);
    }

    bool Scanner::IsAtEnd() const
    {
        return current >= std::size(source);
    }

    char Scanner::Advance()
    {
        current++;
        return source[current - 1];
    }

    char Scanner::Peek() const
    {
        if (IsAtEnd())
        {
            return '\0';
        }

        return source[current];
    }

    char Scanner::PeekNext() const
    {
        if (current + 1 < std::size(source))
        {
            return source[current + 1];
        }

        return '\0';
    }

    void Scanner::AddToken(TokenType t)
    {
        auto txt = Text();
        Token new_token{
            t, // type
            line,
            start,
            txt};

        tokens.push_back(new_token);
        start = current;
    }

    void Scanner::Number()
    {
        while (IsDigit(Peek()) && !IsAtEnd())
        {
            auto c = Advance();
        }
        AddToken(TokenType::number);
    }

    std::string Scanner::Text() const
    {
        return source.substr(start, current - start);
    }

    void Scanner::Indentifer()
    {
        while (IsAlphaNumeric(Peek()) && !IsAtEnd())
        {
            auto c = Advance();
        }

        auto keyword = keywords.find(Text());
        if (keyword != keywords.end())
        {
            AddToken(keyword->second);
        }
        else
        {
            AddToken(TokenType::identifier);
        }
    }

    bool Scanner::Match(char c)
    {
        if (IsAtEnd())
        {
            return false;
        }
        if (source[current] != c)
        {
            return false;
        }

        Advance();

        return true;
    }

    const std::vector<Token> &Scanner::ScanTokens()
    {
        char current_char;
        while (!IsAtEnd())
        {
            char c = Advance();
            switch (c)
            {
            case ' ':
                start=current;
                break;
            case '+':
                AddToken(TokenType::plus);
                break;
            case '-':
                AddToken(TokenType::minus);
                break;
            case '*':
                AddToken(TokenType::asterisk);
                break;
            case '<':
                AddToken(TokenType::smaller_then);
                break;
            case '>':
                AddToken(TokenType::greater_then);
                break;
            case ';':
                AddToken(TokenType::semicolon);
                break;
            case ',':
                AddToken(TokenType::comma);
                break;
            case '#':
                current_char = Peek();
                while (current_char != '\n' && !IsAtEnd())
                {
                    current_char = Advance();
                }
                break;
            case '(':
                AddToken(TokenType::left_paren);
                break;
            case ')':
                AddToken(TokenType::right_paren);
                break;
            case '\n':
                line = line + 1;
                start=current;
                break;
            default:
                if (std::isdigit(c) != 0)
                {
                    Number();
                }
                else if (std::isalpha(c) != 0)
                {
                    Indentifer();
                }
                else
                {
                    // error
                }
                break;
            }
        }

        return tokens;
    }

}