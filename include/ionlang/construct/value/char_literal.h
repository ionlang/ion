#pragma once

#include <ionlang/construct/type/integer_type.h>
#include <ionlang/construct/value.h>

namespace ionlang {
    class Pass;

    struct CharLiteral : Value<IntegerType> {
        char value;

        explicit CharLiteral(char value) noexcept;

        void accept(Pass& visitor) override;
    };
}
