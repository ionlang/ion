#pragma once

#include <optional>
#include <string>
#include <ionshared/misc/helpers.h>
#include "construct.h"
#include "type.h"

namespace ionlang {
    class Pass;

    struct Global : ConstructWithParent<Module>, ionshared::Named {
        std::shared_ptr<Type> type;

        ionshared::OptPtr<Expression<>> value;

        Global(
            std::shared_ptr<Module> parent,
            std::shared_ptr<Type> type,
            std::string name,
            ionshared::OptPtr<Expression<>> value = std::nullopt
        );

        void accept(Pass& visitor) override;

        [[nodiscard]] Ast getChildNodes() override;
    };
}
