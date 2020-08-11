#pragma once

#include <ionlang/passes/pass.h>

namespace ionlang {
    class IonIrCodegenPass : public Pass {
    private:
        /**
         * The currently active IonIR module.
         */
        ionshared::OptPtr<ionir::Module> moduleBuffer;

    public:
        void visit(ionshared::Ptr<Construct> node) override;

        void visitExtern(ionshared::Ptr<Extern> node) override;

        void visitPrototype(ionshared::Ptr<Prototype> node) override;
    };
}
