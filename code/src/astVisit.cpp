#include<visitor.h>

// visitor logic on ast, seperate to keep ast code clean.

namespace infra{
    void NumberExpr::Accept(Visitor& vis) const
    {
        vis.Visit(*this);
    }

    void VariableExpr::Accept(Visitor& vis) const
    {
        vis.Visit(*this);
    }
    
    void BinaryExpr::Accept(Visitor& vis) const
    {
        vis.Visit(*this);
    }

    void CallExpr::Accept(Visitor& vis) const
    {
        vis.Visit(*this);
    }

    void Prototype::Accept(Visitor& vis) const
    {
        vis.Visit(*this);
    }

    void Function::Accept(Visitor& vis) const
    {
        vis.Visit(*this);
    }
}