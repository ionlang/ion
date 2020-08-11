#pragma once

#include <ionshared/container/stack.h>
#include <ionlang/passes/pass.h>

namespace ionlang {
    class IonIrCodegenPass : public Pass {
    private:
        ionshared::Stack<ionshared::Ptr<ionir::Value<>>> valueStack;

        ionshared::Stack<ionshared::Ptr<ionir::Type>> typeStack;

        /**
         * The currently active IonIR module.
         */
        ionshared::OptPtr<ionir::Module> moduleBuffer;

    public:
        explicit IonIrCodegenPass();

        ~IonIrCodegenPass();

        ionshared::Stack<ionir::Value<>> getValueStack() const noexcept;

        ionshared::Stack<ionir::Type> getTypeStack() const noexcept;

        void visit(ionshared::Ptr<Construct> node) override;

        void visitExtern(ionshared::Ptr<Extern> node) override;

        void visitPrototype(ionshared::Ptr<Prototype> node) override;

        void visitType(ionshared::Ptr<Type> node) override;

        void visitIntegerValue(ionshared::Ptr<IntegerValue> node) override;

        void visitCharValue(ionshared::Ptr<CharValue> node) override;

        void visitStringValue(ionshared::Ptr<StringValue> node) override;

        void visitBooleanValue(ionshared::Ptr<BooleanValue> node) override;

    };
}
