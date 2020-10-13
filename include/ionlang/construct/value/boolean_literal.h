#pragma once

#include <ionlang/construct/type/boolean_type.h>
#include <ionlang/construct/expression.h>

namespace ionlang {
    struct Pass;

    struct BooleanLiteral : Expression<BooleanType> {
        bool value;

        explicit BooleanLiteral(bool value);

        void accept(Pass& visitor) override;
    };
}
