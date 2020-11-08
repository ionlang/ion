#pragma once

#include "construct.h"

namespace ionlang {
    struct Pass;

    struct StructDefExpr : Expression<> {
        static std::shared_ptr<StructDefExpr> make(
            const PtrResolvable<StructType>& type,
            const std::vector<std::shared_ptr<Expression<>>>& values = {}
        ) noexcept;

        std::vector<std::shared_ptr<Expression<>>> values;

        // TODO: Should be marked as 'noexcept' but must first ensure Resolvable<StructType> -> Resolvable<Type> flattening (via ->staticCast) never throws.
        explicit StructDefExpr(
            const PtrResolvable<StructType>& type,
            std::vector<std::shared_ptr<Expression<>>> values = {}
        );

        void accept(Pass& visitor) override;

        [[nodiscard]] Ast getChildNodes() override;
    };
}
