#include <codegen.h>
#include <llvm/Support/raw_ostream.h>
#include <notImplmented.h>
#include <fmt/printf.h>
#include <stack>

namespace infra
{
    using namespace llvm;

    Value *IRCodegen::CodeValue()
    {
        auto out = value_cache.back();
        value_cache.pop_back();
        return out; // we can only end with one value
    }

    const std::vector<llvm::Function*>& IRCodegen::CodeFunctions() const
    {
        return function_cache;
    }

    IRCodegen::IRCodegen() : TheContext(),
                             TheModule("llvm jit", TheContext),
                             Builder(TheContext),
                             NameValues()
    {
    }

    void IRCodegen::Visit(const NumberExpr &expr)
    {
        // APFloat holds constant of Arbirtray precision
        auto constant = ConstantFP::get(TheContext, APFloat(expr.Val()));
        value_cache.push_back(constant);
    }

    void IRCodegen::Visit(const VariableExpr &expr)
    {
        Value *v = NameValues[expr.Name()];
        if (v == nullptr)
        {
            throw Error{
                fmt::format("Unable to find variable {}.", expr.Name())};
        }
        value_cache.push_back(v);
    }

    void IRCodegen::Visit(const BinaryExpr &expr)
    {
        expr.Left().Accept(*this);
        Value *left_expr = value_cache.back();
        value_cache.pop_back();
        expr.Right().Accept(*this);
        Value *right_expr = value_cache.back();
        value_cache.pop_back();

        switch (expr.Op())
        {
        case BinaryOperator::ASTERISK:
            value_cache.push_back(Builder.CreateFMul(left_expr, right_expr, "multmp"));
            break;
        case BinaryOperator::MINUS:
            value_cache.push_back(Builder.CreateFSub(left_expr, right_expr, "subtmp"));
            break;
        case BinaryOperator::PLUS:
            value_cache.push_back(Builder.CreateFAdd(left_expr, right_expr, "addtmp"));
            break;
        case BinaryOperator::SMALLER_THEN:
            left_expr = Builder.CreateFCmpULT(left_expr, right_expr, "cmptmp");
            left_expr = Builder.CreateUIToFP(left_expr, Type::getDoubleTy(TheContext), "booltmp");
            value_cache.push_back(left_expr);
            break;
        case BinaryOperator::GREATHER_THEN:
            left_expr = Builder.CreateFCmpUGT(left_expr, right_expr, "cmptmp");
            left_expr = Builder.CreateUIToFP(left_expr, Type::getDoubleTy(TheContext), "booltmp");
            value_cache.push_back(left_expr);
            break;
        default:
            NOT_IMPLEMENTED;
            break;
        }
    }

    void IRCodegen::Visit(const CallExpr &expr)
    {
        llvm::Function *callee_function = TheModule.getFunction(expr.Callee());
        if (callee_function == nullptr)
        {
            throw Error{fmt::format("Unable to find function {}.", expr.Callee())};
        }

        if (std::size(expr.Args()) != callee_function->arg_size())
        {
            throw Error{fmt::format(
                "call to function {} requires {} calls but given {} calls",
                expr.Callee(),
                callee_function->arg_size(),
                std::size(expr.Args()))};
        }

        std::vector<Value *> args;
        for (const auto &arg : expr.Args())
        {
            arg->Accept(*this);
            args.push_back(value_cache.back());
            value_cache.pop_back();
        }

        Value *val = Builder.CreateCall(callee_function, args, "calltmp");
        value_cache.push_back(val);
    }

    void IRCodegen::Visit(const Prototype &expr)
    {
        std::vector<Type *> Doubles(
            std::size(expr.Args()),
            Type::getDoubleTy(TheContext));

        FunctionType *FT = FunctionType::get(
            Type::getDoubleTy(TheContext),
            Doubles,
            false);

        llvm::Function *F = llvm::Function::Create(
            FT,
            llvm::Function::ExternalLinkage,
            expr.Name(),
            &TheModule);

        // optional step: set the argument names
        int i = 0;
        for (auto &arg : F->args())
        {
            arg.setName(expr.Args()[i]);
            i = i + 1;
        }

        function_cache.push_back(F);
    }

    void IRCodegen::Visit(const Function &expr)
    {
        auto proto_name = expr.Proto().Name();
        llvm::Function *TheFunction = TheModule.getFunction(proto_name);

        if (TheFunction == nullptr)
        {
            // Only generate if it does not exit yet.
            expr.Proto().Accept(*this);
            TheFunction = static_cast<llvm::Function *>(function_cache.back());
            function_cache.pop_back();
        }
        else
        {
            if (!TheFunction->empty())
            {
                // The body of the function should be empty.
                throw Error{fmt::format(
                    "Function {} cannot be redefined.",
                    expr.Proto().Name())};
            }
        }

        BasicBlock *BB = BasicBlock::Create(TheContext, "entry", TheFunction);
        Builder.SetInsertPoint(BB);

        NameValues.clear();
        for (auto &arg : TheFunction->args())
        {
            NameValues[arg.getName().str()] = &arg;
        }

        expr.Body().Accept(*this);
        Value *ret_val = value_cache.back();
        value_cache.pop_back();

        if (ret_val != nullptr)
        {
            Builder.CreateRet(ret_val);

            verifyFunction(*TheFunction);

            function_cache.push_back(TheFunction);
            return;
        }

        // error -> remove the function
        TheFunction->eraseFromParent();
    }
}