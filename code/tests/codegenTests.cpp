#include <catch2/catch_all.hpp>
#include <scanner.h>
#include <parser.h>
#include <codegen.h>
#include<llvm/Support/raw_ostream.h>

TEST_CASE("Given_Sum_Find_IR")
{
    using namespace infra;

    std::string source = "3 + 4";
    Scanner scanner(source);
    auto tkns = scanner.ScanTokens();

    Parser parser(tkns);
    auto ast = parser.ParseExpression();

    IRCodegen gen;
    ast->Accept(gen);

    // Gen contains the instance of value so keep it alive.
    auto val = gen.CodeValue();
    std::string val_str;
    auto stream = llvm::raw_string_ostream(val_str);
    val->print(stream);

    std::string expected_val_str = "double 7.000000e+00";

    REQUIRE(val_str == expected_val_str);
}
