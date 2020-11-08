#pragma once

#include <ionlang/construct/pseudo/resolvable.h>
#include "type.h"
#include "expression.h"

namespace ionlang {
    struct Pass;

    struct CastExpr : Expression<> {
        static std::shared_ptr<CastExpr> make(
            const std::shared_ptr<Resolvable<Type>>& type,
            const std::shared_ptr<Expression<>>& value
        ) noexcept;

        std::shared_ptr<Resolvable<Type>> type;

        std::shared_ptr<Expression<>> value;

        CastExpr(
            std::shared_ptr<Resolvable<Type>> type,
            std::shared_ptr<Expression<>> value
        ) noexcept;

        void accept(Pass& visitor) override;

        [[nodiscard]] Ast getChildNodes() override;
    };
}
