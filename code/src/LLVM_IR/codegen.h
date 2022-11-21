#pragma once

#include <map>
#include <string>
#include <memory>
#include <LLVM_IR/ast.h>
#include <LLVM_IR/visitor.h>

#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/ADT/APFloat.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/LegacyPassManager.h>

#include <LLVM_IR/kaleidoscope_jit.h>

namespace infra
{
    class IRCodegen : public Visitor
    {
        const llvm::DataLayout &data_layout;
        std::unique_ptr<llvm::LLVMContext> TheContext;
        std::unique_ptr<llvm::Module> TheModule;
        std::unique_ptr<llvm::IRBuilder<>> Builder;
        std::unique_ptr<llvm::legacy::FunctionPassManager> FPM;
        std::map<std::string, llvm::Value *> NameValues;

        std::vector<llvm::Value *> value_cache;
        std::vector<llvm::Function *> function_cache;

        // This contains the prototypes of all generated functions.
        // Even those not in the module anymore.
        std::map<std::string, std::unique_ptr<Prototype>> function_protos;

        void InitializeModuleAndFunctionPassManager();

        llvm::Function *GetFunction(std::string name);

    public:
        struct Error
        {
            std::string message;
        };

        llvm::Value *CodeValue();
        const std::vector<llvm::Function *> &CodeFunctions() const;

        /// @brief Remove the current module from the code generator, and init a new one.
        llvm::orc::ThreadSafeModule ExtractModule();

        explicit IRCodegen(const llvm::DataLayout &data_layout);
        virtual void Visit(const NumberExpr &expr) override;
        virtual void Visit(const VariableExpr &expr) override;
        virtual void Visit(const BinaryExpr &expr) override;
        virtual void Visit(const CallExpr &expr) override;
        virtual void Visit(const Prototype &expr) override;
        virtual void Visit(const Function &expr) override;
        virtual void Visit(const IfExpr &expr) override;
    };
}