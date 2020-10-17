#pragma once

#include "construct.h"

namespace ionlang {
    struct Pass;

    struct StructDefinition : Expression<> {
        PtrResolvable<Struct> declaration;

        std::vector<ionshared::Ptr<Expression<>>> values;

        explicit StructDefinition(
            const PtrResolvable<Struct>& declaration,
            std::vector<ionshared::Ptr<Expression<>>> values = {}
        ) noexcept;

        void accept(Pass& visitor) override;

        [[nodiscard]] Ast getChildNodes() override;
    };
}
