#pragma once

#include <ast.h>
#include <map>
#include <string>
#include <visitor.h>

#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/ADT/APFloat.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/Value.h>

namespace infra
{
    class IRCodegen : public Visitor
    {
        llvm::LLVMContext TheContext;
        llvm::Module TheModule;
        llvm::IRBuilder<> Builder;
        std::map<std::string, llvm::Value *> NameValues;

        std::vector<llvm::Value *> value_cache;
        std::vector<llvm::Function *> function_cache;

    public:
        struct Error
        {
            std::string message;
        };

        llvm::Value *CodeValue();
        const std::vector<llvm::Function *> &CodeFunctions() const;

        explicit IRCodegen();
        virtual void Visit(const NumberExpr &expr) override;
        virtual void Visit(const VariableExpr &expr) override;
        virtual void Visit(const BinaryExpr &expr) override;
        virtual void Visit(const CallExpr &expr) override;
        virtual void Visit(const Prototype &expr) override;
        virtual void Visit(const Function &expr) override;
    };
}