#pragma once

#include <ionlang/construct/type/boolean_type.h>
#include <ionlang/construct/value.h>

namespace ionlang {
    class Pass;

    struct BooleanLiteral : Value<BooleanType> {
        bool value;

        explicit BooleanLiteral(bool value);

        void accept(Pass& visitor) override;
    };
}
