#pragma once

#include <optional>
#include <string>
#include <ionshared/misc/helpers.h>
#include "construct.h"
#include "type.h"

namespace ionlang {
    struct Pass;

    struct Global : ConstructWithParent<Module>, ionshared::Named {
        static std::shared_ptr<Global> make(
            const std::shared_ptr<Type>& type,
            const std::string& name,
            ionshared::OptPtr<Expression<>> value = std::nullopt
        ) noexcept;

        std::shared_ptr<Type> type;

        ionshared::OptPtr<Expression<>> value;

        Global(
            std::shared_ptr<Type> type,
            std::string name,
            ionshared::OptPtr<Expression<>> value = std::nullopt
        );

        void accept(Pass& visitor) override;

        [[nodiscard]] Ast getChildNodes() override;
    };
}
