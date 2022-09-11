#pragma once
#include<ast.h>

namespace infra
{
    class Visitor
    {
    public:
        virtual void Visit(const NumberExpr &expr);
        virtual void Visit(const VariableExpr &expr);
        virtual void Visit(const BinaryExpr &expr);
        virtual void Visit(const CallExpr &expr);
        virtual void Visit(const Prototype &expr);
        virtual void Visit(const Function &expr);
    };

    void DepthFirstVisit(Visitor&);
}