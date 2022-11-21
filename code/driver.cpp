#include <iostream>
#include <LLVM_IR/parser.h>
#include <LLVM_IR/scanner.h>
#include <LLVM_IR/codegen.h>
#include <LLVM_IR/kaleidoscope_jit.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/Error.h>
#include <fmt/printf.h>
#include <fmt/ranges.h>
 
int main()
{
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    auto jit_expected = llvm::orc::KaleidoscopeJIT::Create();

    if (!jit_expected)
    {
        // auto err = jit_expected.takeError();
        return -1;
    }
    auto &jit = jit_expected.get();
    auto data_layout = jit->getDataLayout();

    infra::IRCodegen gen(data_layout);
    while (true)
    {
        std::string source;
        std::cout << ">";
        std::getline(std::cin, source);

        using namespace infra;

        Scanner scanner(source);
        auto tkns = scanner.ScanTokens();

        Parser parser(tkns);

        while (!parser.IsAtEnd())
        {
            auto t = parser.Peek().type;
            switch (t)
            {
            case TokenType::semicolon:
                break;
            case TokenType::def:
            {
                auto f = parser.ParseDefinition();
                try{
                    f->Accept(gen);
                }
                catch(IRCodegen::Error e)
                {
                    fmt::print("Error when generating code function definition: \n {} \n",
                        e.message);
                    break; // do not continue here
                }
                auto code = gen.CodeFunctions().back();
                code->print(llvm::outs());

                auto H = jit->addModule(llvm::orc::ThreadSafeModule(std::move(gen.ExtractModule())));
                llvm::logAllUnhandledErrors(
                    std::move(H),
                     llvm::errs(),
                     "[unhandled Error when adding function definition to jit] ");

            }
            break;
            case TokenType::ext:
            {
                auto ext_f = parser.ParseExtern();
                try{
                    ext_f->Accept(gen);
                }
                catch(IRCodegen::Error e)
                {
                    fmt::print("Error when generating code external function definition: \n {} \n",
                        e.message);
                        break;
                }
                auto code = gen.CodeFunctions().back();
                code->print(llvm::outs());

                auto H = jit->addModule(llvm::orc::ThreadSafeModule(std::move(gen.ExtractModule())));
                llvm::logAllUnhandledErrors(
                    std::move(H),
                     llvm::errs(),
                     "[unhandled Error when adding exteral to jit] ");
            }
            break;
            default:
            {
                auto f = parser.ParseTopLevelExpr();
                fmt::print("Evaluating expression {} \n", f->Body().ToString());
                try{
                    f->Accept(gen);
                }
                catch(IRCodegen::Error e)
                {
                    fmt::print("Error when generating code top level expression: \n {} \n",
                        e.message);
                    break;
                }
                auto code = gen.CodeFunctions().back();
                code->print(llvm::outs());

                // The resouce_tracker is used to remove the anonimous expression afterwards
                // Otherwise we would get "duplicated" symbol the second time we define
                // Parser::anon_expr
                auto resource_tracker = jit->getMainJITDylib().createResourceTracker();

                // This reset the module in the code generator.
                auto mod = llvm::orc::ThreadSafeModule(std::move(gen.ExtractModule()));
                auto H = jit->addModule(std::move(mod), resource_tracker);
                llvm::logAllUnhandledErrors(
                    std::move(H),
                     llvm::errs(),
                     "[unhandled Error when adding module to jit] ");

                auto expr_symbol = jit->lookup(Parser::anon_expr);
                if(!expr_symbol)
                {
                    // failed to find anom function
                    auto remaining_errs = llvm::handleErrors(expr_symbol.takeError(), 
                        [&](llvm::orc::SymbolsNotFound& snf){
                            fmt::print("Failed to find symbol name {} \n", Parser::anon_expr);
                        });
                    llvm::logAllUnhandledErrors(
                        std::move(remaining_errs),
                         llvm::errs(),
                         "[unhandled JIT lookup Error] ");
                }
                else{
                    // Get the symbol's address and cast it to the right type (takes no
                    // arguments, returns a double) so we can call it as a native function.
                    double (*FP)() = (double (*)())(intptr_t)expr_symbol->getAddress();
                    auto evaluated_output = FP();
                    fmt::print("Evaluated to {:f} \n", evaluated_output);
                }

                auto err_remove_resouce_tracker = resource_tracker->remove();
                llvm::logAllUnhandledErrors(
                    std::move(err_remove_resouce_tracker),
                     llvm::errs(),
                     "[unhandled Error when removing anonimous func from jit] ");
            }
            break;
            }
        }
    }
}