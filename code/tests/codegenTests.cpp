#include <catch2/catch_all.hpp>
#include <LLVM_IR/scanner.h>
#include <LLVM_IR/parser.h>
#include <LLVM_IR/codegen.h>
#include <llvm/Support/raw_ostream.h>
#include <LLVM_IR/kaleidoscope_jit.h>
#include <llvm/Support/TargetSelect.h>

TEST_CASE("Given_Sum_Find_IR")
{
    using namespace infra;

    std::string source = "3 + 4";
    Scanner scanner(source);
    auto tkns = scanner.ScanTokens();

    Parser parser(tkns);
    auto ast = parser.ParseExpression();

    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    auto expected_jit = llvm::orc::KaleidoscopeJIT::Create();
    REQUIRE(expected_jit);

    auto data_layout = expected_jit.get()->getDataLayout();
    IRCodegen gen(data_layout);
    ast->Accept(gen);

    // Gen contains the instance of value so keep it alive.
    auto val = gen.CodeValue();
    std::string val_str;
    auto stream = llvm::raw_string_ostream(val_str);
    val->print(stream);

    std::string expected_val_str = "double 7.000000e+00";

    REQUIRE(val_str == expected_val_str);
}
