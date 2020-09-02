#pragma once

#include <ionlang/passes/pass.h>

namespace ionlang {
    class DeadCodeEliminationPass : public Pass {
    public:
        void visitBlock(ionshared::Ptr<Block> node) override;

        // TODO: Optimize assignment to a dead variable (alloca but never used).
    };
}
