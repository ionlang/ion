#pragma once

#include <ionshared/container/stack.h>
#include <ionlang/passes/pass.h>

namespace ionlang {
    class IonIrCodegenPass : public Pass {
    private:
        ionshared::PtrSymbolTable<ionir::Module> modules;

        ionshared::Stack<ionshared::Ptr<ionir::Construct>> constructStack;

        ionshared::Stack<ionshared::Ptr<ionir::Type>> typeStack;

        /**
         * The currently active IonIR module.
         */
        ionshared::OptPtr<ionir::Module> moduleBuffer;

        ionshared::OptPtr<ionir::Function> functionBuffer;

        void requireModule();

        void requireFunction();

    public:
        explicit IonIrCodegenPass(
            ionshared::PtrSymbolTable<ionir::Module> modules =
                std::make_shared<ionshared::SymbolTable<ionshared::Ptr<ionir::Module>>>()
        );

        ~IonIrCodegenPass();

        ionshared::Ptr<ionshared::SymbolTable<ionshared::Ptr<ionir::Module>>> getModules() const;

        ionshared::Stack<ionshared::Ptr<ionir::Construct>> getConstructStack() const noexcept;

        ionshared::Stack<ionshared::Ptr<ionir::Type>> getTypeStack() const noexcept;

        ionshared::OptPtr<ionir::Module> getModuleBuffer() const;

        bool setModuleBuffer(const std::string &id);

        void visit(ionshared::Ptr<Construct> node) override;

        void visitModule(ionshared::Ptr<Module> node) override;

        void visitFunction(ionshared::Ptr<Function> node) override;

        void visitExtern(ionshared::Ptr<Extern> node) override;

        void visitFunctionBody(ionshared::Ptr<FunctionBody> node) override;

        void visitPrototype(ionshared::Ptr<Prototype> node) override;

        void visitIntegerValue(ionshared::Ptr<IntegerValue> node) override;

        void visitCharValue(ionshared::Ptr<CharValue> node) override;

        void visitStringValue(ionshared::Ptr<StringValue> node) override;

        void visitBooleanValue(ionshared::Ptr<BooleanValue> node) override;

        void visitGlobal(ionshared::Ptr<Global> node) override;

        void visitType(ionshared::Ptr<Type> node) override;

        void visitIntegerType(ionshared::Ptr<IntegerType> node) override;

        void visitBooleanType(ionshared::Ptr<BooleanType> node) override;

        void visitVoidType(ionshared::Ptr<VoidType> node) override;
    };
}
