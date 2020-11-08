#pragma once

#include "type.h"
#include "construct.h"
#include "expression.h"

namespace ionlang {
    struct Pass;

    struct Cast : Construct {
        static std::shared_ptr<Cast> make(
            const std::shared_ptr<Type>& type,
            const std::shared_ptr<Expression<>>& value
        ) noexcept;

        std::shared_ptr<Type> type;

        std::shared_ptr<Expression<>> value;

        Cast(
            std::shared_ptr<Type> type,
            std::shared_ptr<Expression<>> value
        ) noexcept;

        void accept(Pass& visitor) override;

        [[nodiscard]] Ast getChildNodes() override;
    };
}
