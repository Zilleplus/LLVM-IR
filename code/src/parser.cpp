#include <parser.h>
#include <numeric>
#include <algorithm>
#include <fmt/printf.h>
#include <cassert>

namespace infra
{
    std::string Parser::anon_expr  = "__anon_expr";

    Parser::Parser(const std::vector<Token> &tokens) : tokens(tokens), current(0)
    {
        binOpPrecedence[TokenType::smaller_then] = 10;
        binOpPrecedence[TokenType::greater_then] = 10;
        binOpPrecedence[TokenType::plus] = 20;
        binOpPrecedence[TokenType::minus] = 20;
        binOpPrecedence[TokenType::asterisk] = 40;
    }

    void Parser::Error::Print() const
    {
        fmt::print("Error on parsing line={} position={} \n error={}",
                   line_number,
                   position,
                   message);
    }

    std::vector<std::unique_ptr<Ast>> Parser::Parse()
    {
        std::vector<std::unique_ptr<Ast>> out;

        try
        {
            while (!IsAtEnd())
            {
                auto t = Peek().type;
                switch (t)
                {
                case TokenType::semicolon:
                    break;
                case TokenType::def:
                    out.push_back(ParseDefinition());
                    break;
                case TokenType::ext:
                    out.push_back(ParseExtern());
                    break;
                default:
                    out.push_back(ParseTopLevelExpr());
                    break;
                }
            }
        }
        catch (Parser::Error e)
        {
            e.Print();
        }

        return out;
    }

    Token Parser::Advance()
    {
        auto out = tokens[current];
        current = current + 1;

        return out;
    }

    Token Parser::Peek() const
    {
        return tokens[current];
    }

    std::optional<Token> Parser::PeekNext() const
    {
        auto next = current + 1;
        if (next < std::size(tokens))
        {
            return tokens[next];
        }
        return {};
    }

    /// @brief Check if the next token type equals some type
    bool Parser::Check(TokenType t) const
    {
        if (IsAtEnd())
        {
            return false;
        }

        return t == Peek().type;
    }

    /// @brief Are there more tokens to be consumed?
    bool Parser::IsAtEnd() const
    {
        return current >= std::size(tokens);
    }

    Token Parser::Consume(TokenType type, std::string error_message)
    {
        if (Peek().type == type)
        {
            return Advance();
        }
        else
        {
            throw Error(Peek(), error_message);
        }
    }

    bool Parser::Match(std::initializer_list<TokenType> tokens)
    {
        const bool c = std::any_of(
            tokens.begin(),
            tokens.end(),
            [&](auto x)
            { return Parser::Check(x); });

        if (c)
        {
            Advance();
        }

        return c;
    }

    int Parser::Precedence()
    {
        if (IsAtEnd())
        {
            return -1;
        }

        auto t = Peek();
        auto prec_it = binOpPrecedence.find(t.type);
        if (prec_it == binOpPrecedence.end())
        {
            return -1; // not a bin operator
        }

        return prec_it->second;
    }

    std::unique_ptr<Expr> Parser::ParseExpression()
    {
        auto e = ParsePrimary();
        if (Precedence() < 0)
        {
            // not a binary operator
            return e;
        }

        e = ParseRHSExpression(0, std::move(e));

        return e;
    }

    std::unique_ptr<Expr> Parser::ParseRHSExpression(
        int precedence,
        std::unique_ptr<Expr> lhs)
    {

        while (true)
        {
            auto prec_op = Precedence();

            if (prec_op < precedence)
            {
                return lhs;
            }

            auto op = Advance();

            auto rhs = ParsePrimary();
            auto next_prec_op = Precedence();
            if (next_prec_op > prec_op)
            {
                rhs = ParseRHSExpression(prec_op + 1, std::move(rhs));
            }
            lhs = std::make_unique<BinaryExpr>(
                BinOpFromTokenType(op.type),
                std::move(lhs),
                std::move(rhs));
        }

        return lhs;
    }

    std::unique_ptr<Expr> Parser::ParsePrimary()
    {
        if (Check(TokenType::number))
        {
            return ParseNumber();
        }
        else if (Check(TokenType::identifier))
        {
            return ParseIndentifier();
        }

        // Not a number of identifier? something is wrong.

        const auto current_token = Peek();
        throw Error(
            current_token,
            fmt::format("Unexpected token {} when expecting identifier or number",
                        ToString(current_token)));
    }

    std::unique_ptr<Expr> Parser::ParseNumber()
    {
        auto t = Advance();
        if (t.type != TokenType::number)
        {
            throw Error(
                t,
                fmt::format("Unexpected token {t} when expecting number",
                            ToString(t)));
        }

        auto num_val = std::stod(t.txt);
        return std::make_unique<NumberExpr>(num_val);
    }

    std::unique_ptr<Expr> Parser::ParseIndentifier()
    {
        auto t = Advance();
        assert(t.type == TokenType::identifier);
        if (!Check(TokenType::left_paren))
        {
            return std::make_unique<VariableExpr>(t.txt);
        }

        // it's a call with args
        Advance(); // consume '('
        std::vector<std::unique_ptr<Expr>> args;
        while (!Check(TokenType::right_paren))
        {
            auto arg = ParsePrimary();
            args.push_back(std::move(arg));
            if (!Match({TokenType::comma}))
            {
                break;
            }
        }
        Advance(); // consume ')'
        return std::make_unique<CallExpr>(t.txt, std::move(args));
    }

    std::unique_ptr<Prototype> Parser::ParsePrototype()
    {
        auto name_token = Consume(
            TokenType::identifier,
            fmt::format("Expected function name at the start of prototype"));
        const auto left_paren_token = Consume(
            TokenType::left_paren,
            fmt::format("Expected left paren after function name"));

        std::vector<std::string> args;

        auto t = left_paren_token;
        while (true)
        {
            if (IsAtEnd())
            {
                auto error = fmt::format(
                    "Un expected ending, expected closing paren of left paren at line={} and position={}",
                    left_paren_token.line_number, left_paren_token.position);
                throw Error(t, error);
            }
            t = Advance();

            if (t.type == TokenType::right_paren)
            {
                return std::make_unique<Prototype>(name_token.txt, args);
            }
            else if (t.type == TokenType::identifier)
            {
                args.push_back(t.txt);
                Match({TokenType::comma}); // consume the comma if it's there.
            }
            else
            {
                auto invalid_argument = fmt::format("Invalid function argument, should be number of identifier.");
                throw Error(t, invalid_argument);
            }
        }
    }

    std::unique_ptr<Function> Parser::ParseDefinition()
    {
        auto def_token = Consume(
            TokenType::def,
            fmt::format("Expected 'def' at start of function definition"));

        auto proto = ParsePrototype();
        auto body = ParseExpression();

        return std::make_unique<Function>(std::move(proto), std::move(body));
    }

    std::unique_ptr<Prototype> Parser::ParseExtern()
    {
        auto extern_token = Consume(
            TokenType::ext,
            fmt::format("Expected extern"));

        return ParsePrototype();
    }

    std::unique_ptr<Function> Parser::ParseTopLevelExpr()
    {
        auto proto = std::make_unique<Prototype>(
            Parser::anon_expr, std::vector<std::string>{});

        auto body = ParseExpression();

        return std::make_unique<Function>(std::move(proto), std::move(body));
    }
}