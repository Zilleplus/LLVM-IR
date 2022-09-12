#include <catch2/catch_all.hpp>
#include <parser.h>
#include <scanner.h>

using namespace infra;

static std::vector<TokenType> Types(const std::vector<Token>& tks)
{
    std::vector<TokenType> out;
    for(auto t : tks)
    {
        out.push_back(t.type);
    }

    return out;
}

static std::vector<std::pair<std::string, std::string>> get_data_expression()
{
    std::vector<std::pair<std::string, std::string>> out;

    {
        std::string input = "5 + 3";
        std::string exp = "(+ 5 3)";
        out.emplace_back(input, exp);
    }
    {
        std::string input = "5+3*2";
        std::string exp = "(+ 5 (* 3 2))";
        out.emplace_back(input, exp);
    }
    {
        std::string input = "5*3+2";
        std::string exp = "(+ (* 5 3) 2)";
        out.emplace_back(input, exp);
    }

    return out;
}

TEST_CASE("Given_Expression_In_Source_Find_Expression")
{
    for (auto [input, exp] : get_data_expression())
    {
        auto scanner = Scanner{input};
        auto tokens = scanner.ScanTokens();
        Parser sub(tokens);
        auto res = sub.ParseExpression();
        auto res_str = res->ToString();

        REQUIRE(res_str == exp);
    }
}

TEST_CASE("Given_Function_In_Source_Find_AST")
{
    std::string source =
        "def double(x)\n"
        "2 * x";
    Scanner scanner(source);
    auto tks = scanner.ScanTokens();

    std::vector<TokenType> types = {
        TokenType::def,
        TokenType::identifier,
        TokenType::left_paren,
        TokenType::identifier,
        TokenType::right_paren,
        TokenType::number,
        TokenType::asterisk,
        TokenType::identifier
    };
    auto res_token_types = Types(tks);
    REQUIRE(res_token_types == types);

    Parser parser(tks);
    std::unique_ptr<Function> res = parser.ParseDefinition();

    std::string exp_str = "(function (prototype double x) (* 2 x))";
    auto res_str = res->ToString();

    REQUIRE(res_str == exp_str);
}