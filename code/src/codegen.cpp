#include <codegen.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <notImplmented.h>
#include <fmt/printf.h>
#include <stack>
#include <memory>

namespace infra
{
    using namespace llvm;

    Value *IRCodegen::CodeValue()
    {
        auto out = value_cache.back();
        value_cache.pop_back();
        return out; // we can only end with one value
    }

    const std::vector<llvm::Function *> &IRCodegen::CodeFunctions() const
    {
        return function_cache;
    }

    IRCodegen::IRCodegen(const DataLayout &data_layout) : data_layout(data_layout),
                                                          TheModule(nullptr),
                                                          FPM(nullptr),
                                                          NameValues()
    {
        InitializeModuleAndFunctionPassManager();
    }

    llvm::orc::ThreadSafeModule IRCodegen::ExtractModule()
    {
        auto m = std::move(TheModule);
        auto c = std::move(TheContext);
        InitializeModuleAndFunctionPassManager();
        return llvm::orc::ThreadSafeModule(std::move(m), std::move(c));
    }

    void IRCodegen::InitializeModuleAndFunctionPassManager()
    {
        TheContext = std::make_unique<llvm::LLVMContext>();
        TheModule = std::make_unique<Module>("llvm jit", *TheContext);
        TheModule->setDataLayout(data_layout);

        Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);

        FPM = std::make_unique<llvm::legacy::FunctionPassManager>(TheModule.get());
        FPM->add(createInstructionCombiningPass());
        FPM->add(createReassociatePass());
        FPM->add(createGVNPass());
        FPM->add(createCFGSimplificationPass());
        FPM->doInitialization();
    }

    void IRCodegen::Visit(const NumberExpr &expr)
    {
        // APFloat holds constant of Arbirtray precision
        auto constant = ConstantFP::get(*TheContext, APFloat(expr.Val()));
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
            value_cache.push_back(Builder->CreateFMul(left_expr, right_expr, "multmp"));
            break;
        case BinaryOperator::MINUS:
            value_cache.push_back(Builder->CreateFSub(left_expr, right_expr, "subtmp"));
            break;
        case BinaryOperator::PLUS:
            value_cache.push_back(Builder->CreateFAdd(left_expr, right_expr, "addtmp"));
            break;
        case BinaryOperator::SMALLER_THEN:
            left_expr = Builder->CreateFCmpULT(left_expr, right_expr, "cmptmp");
            left_expr = Builder->CreateUIToFP(left_expr, Type::getDoubleTy(*TheContext), "booltmp");
            value_cache.push_back(left_expr);
            break;
        case BinaryOperator::GREATHER_THEN:
            left_expr = Builder->CreateFCmpUGT(left_expr, right_expr, "cmptmp");
            left_expr = Builder->CreateUIToFP(left_expr, Type::getDoubleTy(*TheContext), "booltmp");
            value_cache.push_back(left_expr);
            break;
        default:
            NOT_IMPLEMENTED;
            break;
        }
    }

    void IRCodegen::Visit(const CallExpr &expr)
    {
        llvm::Function *callee_function = GetFunction(expr.Callee());
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

        Value *val = Builder->CreateCall(callee_function, args, "calltmp");
        value_cache.push_back(val);
    }

    void IRCodegen::Visit(const Prototype &expr)
    {
        std::vector<Type *> Doubles(
            std::size(expr.Args()),
            Type::getDoubleTy(*TheContext));

        FunctionType *FT = FunctionType::get(
            Type::getDoubleTy(*TheContext),
            Doubles,
            false);

        llvm::Function *F = llvm::Function::Create(
            FT,
            llvm::Function::ExternalLinkage,
            expr.Name(),
            TheModule.get());

        // optional step: set the argument names
        int i = 0;
        for (auto &arg : F->args())
        {
            arg.setName(expr.Args()[i]);
            i = i + 1;
        }

        function_cache.push_back(F);

        // keep the prototype between modules:
        function_protos[expr.Name()] = std::make_unique<Prototype>(expr);
    }

    void IRCodegen::Visit(const Function &expr)
    {
        auto proto_name = expr.Proto().Name();
        llvm::Function *TheFunction = GetFunction(proto_name);

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

        BasicBlock *BB = BasicBlock::Create(*TheContext, "entry", TheFunction);
        Builder->SetInsertPoint(BB);

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
            Builder->CreateRet(ret_val);

            verifyFunction(*TheFunction);

            FPM->run(*TheFunction);

            function_cache.push_back(TheFunction);
            return;
        }

        // error -> remove the function
        TheFunction->eraseFromParent();
    }

    llvm::Function *IRCodegen::GetFunction(std::string name)
    {
        if (auto *F = TheModule->getFunction(name))
        // function is in current module
        {
            return F;
        }

        auto proto_previous_module = function_protos.find(name);
        if (proto_previous_module != function_protos.end())
        {
            // The function/extern was defined in some previous module.
            // Get the prototype expression, and generate the llvm
            // function using visit.
            const auto &p = *proto_previous_module;
            Visit(*p.second);
            // The function prototype is now in the module, so we can use it.
            llvm::Function *f = function_cache.back();
            function_cache.pop_back();
            return f;
        }

        // function does not exist
        return nullptr;
    }

}