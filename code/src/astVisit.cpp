#include<visitor.h>

// visitor logic on ast, seperate to keep ast code clean.

namespace infra{
    void NumberExpr::Visit(Visitor& vis)
    {
        vis.Visit(*this);
    }

    void VariableExpr::Visit(Visitor& vis)
    {
        vis.Visit(*this);
    }
    
    void BinaryExpr::Visit(Visitor& vis)
    {
        vis.Visit(*this);
    }

    void CallExpr::Visit(Visitor& vis)
    {
        vis.Visit(*this);
    }

    void Prototype::Visit(Visitor& vis)
    {
        vis.Visit(*this);
    }

    void Function::Visit(Visitor& vis)
    {
        vis.Visit(*this);
    }
}