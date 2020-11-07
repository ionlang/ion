#pragma once

#include "type.h"
#include "construct.h"

namespace ionlang {
    struct Pass;

    struct Cast : Construct {
        static std::shared_ptr<Cast> make(
            const std::shared_ptr<Type>& type
        ) noexcept;

        std::shared_ptr<Type> type;

        explicit Cast(std::shared_ptr<Type> type);

        void accept(Pass& visitor) override;

        [[nodiscard]] Ast getChildNodes() override;
    };
}
