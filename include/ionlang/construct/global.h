#pragma once

#include <optional>
#include <string>
#include <ionshared/misc/helpers.h>
#include "value.h"
#include "construct.h"
#include "type.h"

namespace ionlang {
    class Pass;

    struct Global : ConstructWithParent<Module>, ionshared::Named {
        ionshared::Ptr<Type> type;

        ionshared::OptPtr<Value<>> value;

        Global(
            ionshared::Ptr<Module> parent,
            ionshared::Ptr<Type> type,
            std::string name,
            ionshared::OptPtr<Value<>> value = std::nullopt
        );

        void accept(Pass &visitor) override;

        [[nodiscard]] Ast getChildNodes() override;
    };
}
