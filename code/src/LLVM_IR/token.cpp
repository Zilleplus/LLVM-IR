#include <LLVM_IR/token.h>
#include <fmt/format.h>

namespace infra
{

    std::string ToString(TokenType t)
    {
        switch (t)
        {
        case TokenType::eof:
            return "EOF";
        case TokenType::def:
            return "def";
        case TokenType::ext:
            return "extern";
        case TokenType::identifier:
            return "identifier";
        case TokenType::number:
            return "number";
        case TokenType::left_paren:
            return "(";
        case TokenType::right_paren:
            return ")";
        case TokenType::plus:
            return "+";
        case TokenType::minus:
            return "-";
        case TokenType::smaller_then:
            return "<";
        case TokenType::greater_then:
            return ">";
        case TokenType::if_:
            return "if";
        case TokenType::else_:
            return "else";
        case TokenType::then:
            return "then";
        default:
            return "Unknown token type";
        }
    }

    std::string ToString(const Token &t)
    {
        return fmt::format(
            "[type={}; line_number={}; position{}, txt={}]",
             t.type,
             t.line_number,
             t.position, 
             t.txt);
    }

} // namespace infra