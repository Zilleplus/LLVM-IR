#include<iostream>
#include<parser.h>
#include<scanner.h>
#include<codegen.h>
#include<llvm/Support/raw_ostream.h>

int main(){
    infra::IRCodegen gen;
    while(true)
    {
        std::string source;
        std::cout << ">";
        std::getline(std::cin, source);
        
        using namespace infra;

        Scanner scanner(source);
        auto tkns = scanner.ScanTokens();

        Parser parser(tkns);
        auto ast = parser.Parse();

        for(const auto& a : ast)
        {
            a->Accept(gen);
            auto fun = gen.CodeFunctions().back();
            
            fun->print(llvm::outs());
        }
    }
}