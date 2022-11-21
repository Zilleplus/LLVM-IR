#pragma once
#include <LLVM_IR/ast.h>

namespace infra
{
    class Visitor
    {
    public:
        virtual void Visit(const NumberExpr &expr) = 0;
        virtual void Visit(const VariableExpr &expr) = 0;
        virtual void Visit(const BinaryExpr &expr) = 0;
        virtual void Visit(const CallExpr &expr) = 0;
        virtual void Visit(const Prototype &expr) = 0;
        virtual void Visit(const Function &expr) = 0;
        virtual void Visit(const IfExpr &expr) = 0;
    };

    void DepthFirstVisit(Visitor &);
}