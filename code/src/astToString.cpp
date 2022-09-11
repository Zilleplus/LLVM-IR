#include <ast.h>
#include <fmt/format.h>

namespace infra
{
    std::string NumberExpr::ToString() const
    {
        return fmt::format("({})", val);
    }

    std::string VariableExpr::ToString() const
    {
        return fmt::format("({})", this->name);
    }

    std::string BinaryExpr::ToString() const
    {
        return fmt::format(
            "({} {} {})",
            infra::ToString(this->op),
            this->left->ToString(),
            this->right->ToString());
    }

    std::string CallExpr::ToString() const
    {
        std::vector<std::string> args_str;
        for(const auto& a : this->args)
        {
            args_str.push_back(a->ToString());
        }
        return fmt::format("(call {} {})", this->callee, fmt::join(args_str, ", "));
    }

    std::string Prototype::ToString() const
    {
        return fmt::format("(prototype {} {})", this->name, fmt::join(this->args, ", "));
    }

    std::string Function::ToString() const
    {
        return fmt::format("(function {} {})", this->proto->ToString(), this->body->ToString());
    }
}