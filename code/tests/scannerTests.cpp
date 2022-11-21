#include <catch2/catch_all.hpp>
#include <LLVM_IR/scanner.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <algorithm>

using namespace infra;

TEST_CASE("Given_Numbers_In_Source_Find_Number_Token")
{
    std::vector<std::pair<std::string, std::string>> nums_text{
        {"1", "1"},
        {" 1", "1"},
        {"1 ", "1"},
        {"12", "12"},
    };

    for (const auto &[input, exp] : nums_text)
    {
        Scanner s(input);
        auto res = s.ScanTokens();

        REQUIRE(std::size(res) == 1);

        Token res_token = res.front();
        auto log = fmt::format(
            "input='{}' expected='{}' res='{}'",
            input,
            exp,
            res_token.txt);
        INFO(log);

        REQUIRE(res_token.type == TokenType::number);
        REQUIRE(res_token.txt == exp);
    }
}

TEST_CASE("Given_Text_Find_TokenType")
{
    std::vector<std::pair<std::string, infra::TokenType>> nums_text{
        {"def", TokenType::def},
        {"extern", TokenType::ext},
        {"(", TokenType::left_paren},
        {")", TokenType::right_paren},
        {"+", TokenType::plus},
        {"-", TokenType::minus},
        {"<", TokenType::smaller_then},
        {">", TokenType::greater_then},
        {"if", TokenType::if_},
        {"then", TokenType::then},
        {"else", TokenType::else_},
    };

    for (const auto &[input, exp] : nums_text)
    {
        Scanner s(input);
        auto res = s.ScanTokens();

        std::vector<std::string> res_str;
        std::transform(res.begin(), res.end(),
                       std::back_inserter(res_str),
                       [](const Token &t)
                       { return infra::ToString(t); });

        auto log = fmt::format(
            "input='{}' expected='{}' res=[{}] ",
            input,
            ToString(exp),
            fmt::join(res_str, ", "));
        INFO(log);

        REQUIRE(std::size(res) == 1);
        Token res_token = res.front();
        REQUIRE(res_token.type == exp);
    }
}

TEST_CASE("Given_Bin_Op_Text_Find_Tokens")
{
    std::vector<std::pair<std::string, std::vector<infra::TokenType>>> nums_text{
        {"1+2*3", {TokenType::number, TokenType::plus, TokenType::number, TokenType::asterisk, TokenType::number}},
    };

    for(auto [input, exp] : nums_text)
    {
        Scanner s(input);
        auto res= s.ScanTokens();
        std::vector<infra::TokenType> res_tok_types;
        for(auto t : res)
        {
            res_tok_types.push_back(t.type);
        }

        REQUIRE(res_tok_types == exp);
    }
}

TEST_CASE("Given_Example_Code_Find_Tokens")
{
    std::string source =
        "/# Compute the x'th fibonacci number.\n"
        "def fib(x)\n"
        "if x < 3 then\n"
        "1\n"
        "else\n"
        "fib(x-1) + fib(x-2)\n"
        "/# The expression wich computes the 40th number\n"
        "fib(40);\n";

    Scanner scanner(source);
    auto tokens = scanner.ScanTokens();

    REQUIRE(!std::empty(tokens));
}