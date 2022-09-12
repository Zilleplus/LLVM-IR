#pragma once

#include <token.h>
#include <ast.h>
#include <vector>
#include <optional>
#include <map>

namespace infra
{
    class Parser
    {
        const std::vector<Token> &tokens;
        int current;
        std::map<TokenType, int> binOpPrecedence;
    public:
        explicit Parser(const std::vector<Token> &tokens);

        std::vector<std::unique_ptr<Ast>> Parse();

        std::unique_ptr<Expr> ParseExpression();

        std::unique_ptr<Expr> ParseRHSExpression(int precedence, std::unique_ptr<Expr> lhs);

        std::unique_ptr<Expr> ParsePrimary();

        std::unique_ptr<Expr> ParseNumber();

        std::unique_ptr<Expr> ParseIndentifier();

        std::unique_ptr<Prototype> ParsePrototype();

        std::unique_ptr<Function> ParseDefinition();

        std::unique_ptr<Prototype> ParseExtern();

        std::unique_ptr<Function> ParseTopLevelExpr();

    private:
        struct Error{
            int line_number;
            int position;
            std::string message;
            Error(const Token& t, std::string message) : 
                line_number(t.line_number),
                position(t.position),
                message(std::move(message)) {}
            void Print() const;
        };

        /// @brief Consume the current token.
        Token Advance();

        /// @brief Get the current token without consuming it
        Token Peek() const;

        std::optional<Token> PeekNext() const;

        /// @brief Check if the next token type equals some type
        bool Check(TokenType) const;

        /// @brief Are there more tokens to be consumed?
        bool IsAtEnd() const;

        /// @brief Check if the next token equals one of the provided token types. Consume the token on match.
        bool Match(std::initializer_list<TokenType> tokens);

        /// @brief Consume a token of certain type or throw
        Token Consume(TokenType type, std::string error_message);

        /// @brief Get binary precedence
        int Precedence();
    };
} // namespace infra