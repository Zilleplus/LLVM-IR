#pragma once

#include <string>
#include <memory>
#include <vector>
#include <string>

namespace infra
{
    class Visitor;

    class Ast
    {
    public:
        virtual ~Ast() {}
        virtual void Visit(Visitor &vis);
        virtual std::string ToString() const;
    };

    class Expr : public Ast
    {
    public:
        virtual ~Expr() {}
    };

    class NumberExpr : public Expr
    {
        double val;

    public:
        explicit NumberExpr(double val) : val(val) {}
        virtual void Visit(Visitor &vis) override;
        virtual std::string ToString() const override;
    };

    class VariableExpr : public Expr
    {
        std::string name;

    public:
        explicit VariableExpr(std::string name) : name(name) {}
        virtual void Visit(Visitor &vis) override;
        virtual std::string ToString() const override;
    };

    enum class BinaryOperator
    {
        PLUS

    };

    inline std::string ToString(BinaryOperator op){
        // At the moment only one binary operator is supported
        return "+";
    }

    class BinaryExpr : public Expr
    {
        BinaryOperator op;
        std::unique_ptr<Expr> left;
        std::unique_ptr<Expr> right;

    public:
        explicit BinaryExpr(
            BinaryOperator op,
            std::unique_ptr<Expr> left,
            std::unique_ptr<Expr> right) : op(op), left(std::move(left)), right(std::move(right))
        {
        }

        virtual void Visit(Visitor &vis) override;
        virtual std::string ToString() const override;
    };

    class CallExpr : public Expr
    {
        std::string callee;
        std::vector<std::unique_ptr<Expr>> args;

    public:
        explicit CallExpr(const std::string &callee, std::vector<std::unique_ptr<Expr>> args)
            : callee(callee), args(std::move(args)) {}
        virtual void Visit(Visitor &vis) override;
        virtual std::string ToString() const override;
    };

    class Prototype : Ast
    {
        std::string name;
        std::vector<std::string> args;

    public:
        explicit Prototype(const std::string name, std::vector<std::string> args) : name(name), args(std::move(args))
        {
        }

        const std::string &Name() const &
        {
            return name;
        }

        std::string Name() &&
        {
            return std::move(name);
        }

        virtual void Visit(Visitor &vis) override;
        virtual std::string ToString() const override;
    };

    class Function : Ast
    {
        std::unique_ptr<Prototype> proto;
        std::unique_ptr<Expr> body;

    public:
        explicit Function(
            std::unique_ptr<Prototype> proto,
            std::unique_ptr<Expr> body)
            : proto(std::move(proto)), body(std::move(body))
        {
        }

        virtual void Visit(Visitor &vis) override;
        virtual std::string ToString() const override;
    };
} // namespace infra