#pragma once

#include <ionlang/misc/helpers.h>
#include <ionlang/passes/pass.h>

namespace ionlang {
    class EntryPointCheckPass : public Pass {
    private:
        ionshared::OptPtr<Function> entryFunction;

    public:
        void finish() override;

        void visitFunction(ionshared::Ptr<Function> node) override;
    };
}
