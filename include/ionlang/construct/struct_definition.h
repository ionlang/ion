#pragma once

#include "construct.h"

namespace ionlang {
    struct Pass;

    struct StructDefinition : Expression<> {
        static std::shared_ptr<StructDefinition> make(
            const PtrResolvable<Struct>& declaration,
            std::vector<std::shared_ptr<Expression<>>> values = {}
        ) noexcept;

        PtrResolvable<Struct> declaration;

        std::vector<std::shared_ptr<Expression<>>> values;

        explicit StructDefinition(
            const PtrResolvable<Struct>& declaration,
            std::vector<std::shared_ptr<Expression<>>> values = {}
        ) noexcept;

        void accept(Pass& visitor) override;

        [[nodiscard]] Ast getChildNodes() override;
    };
}
