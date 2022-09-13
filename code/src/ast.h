#pragma once

#include <string>
#include <memory>
#include <vector>
#include <token.h>

namespace infra
{
    class Visitor;

    class Ast
    {
    public:
        virtual ~Ast() {}
        virtual void Accept(Visitor &vis) const = 0;
        virtual std::string ToString() const = 0;
    };

    class Expr : public Ast
    {
    };

    class NumberExpr : public Expr
    {
        double val;

    public:
        explicit NumberExpr(double val) : val(val) {}
        virtual void Accept(Visitor &vis) const override;
        virtual std::string ToString() const override;
        double Val() const { return val; }
    };

    class VariableExpr : public Expr
    {
        std::string name;

    public:
        explicit VariableExpr(std::string name) : name(name) {}
        virtual void Accept(Visitor &vis) const override;
        virtual std::string ToString() const override;

        std::string Name() const { return name; }
    };

    enum class BinaryOperator
    {
        PLUS,
        MINUS,
        ASTERISK,
        SMALLER_THEN,
        GREATHER_THEN
    };

    inline std::string ToString(BinaryOperator op)
    {
        switch (op)
        {
        case BinaryOperator::PLUS:
            return "+";
        case BinaryOperator::MINUS:
            return "-";
        case BinaryOperator::ASTERISK:
            return "*";
        case BinaryOperator::SMALLER_THEN:
            return "<";
        case BinaryOperator::GREATHER_THEN:
            return ">";
        default:
            return "unknown binary operator";
        }
    }

    inline BinaryOperator BinOpFromTokenType(TokenType t)
    {
        switch (t)
        {
        case TokenType::plus:
            return BinaryOperator::PLUS;
        case TokenType::minus:
            return BinaryOperator::MINUS;
        case TokenType::asterisk:
            return BinaryOperator::ASTERISK;
        default:
            return BinaryOperator::PLUS;
        }
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

        virtual void Accept(Visitor &vis) const override;
        virtual std::string ToString() const override;

        const Expr &Left() const
        {
            return *left;
        }
        const Expr &Right() const
        {
            return *right;
        }

        const BinaryOperator Op() const
        {
            return op;
        }
    };

    class CallExpr : public Expr
    {
        std::string callee;
        std::vector<std::unique_ptr<Expr>> args;

    public:
        explicit CallExpr(const std::string &callee, std::vector<std::unique_ptr<Expr>> args)
            : callee(callee), args(std::move(args)) {}
        virtual void Accept(Visitor &vis) const override;
        virtual std::string ToString() const override;

        std::string Callee() const
        {
            return callee;
        }

        const std::vector<std::unique_ptr<Expr>> &Args() const
        {
            return args;
        }
    };

    class Prototype : public Ast
    {
        std::string name;
        std::vector<std::string> args;

    public:
        explicit Prototype(
            const std::string &name, std::vector<std::string> args)
            : name(name), args(std::move(args))
        {
        }

        std::string Name() const
        {
            return name;
        }

        const std::vector<std::string> &Args() const
        {
            return args;
        }

        virtual void Accept(Visitor &vis) const override;
        virtual std::string ToString() const override;
    };

    class Function : public Ast
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

        const Prototype &Proto() const
        {
            return *proto;
        }

        const Expr &Body() const
        {
            return *body;
        }

        virtual void Accept(Visitor &vis) const override;
        virtual std::string ToString() const override;
    };
} // namespace infra