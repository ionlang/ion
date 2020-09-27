#pragma once

#include <ionlang/construct/value.h>

namespace ionlang {
    class Pass;

    struct BooleanLiteral : Value<> {
        bool value;

        explicit BooleanLiteral(bool value);

        void accept(Pass &visitor) override;
    };
}
