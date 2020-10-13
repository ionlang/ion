#pragma once

#include <ionlang/construct/type/integer_type.h>
#include <ionlang/construct/expression.h>

namespace ionlang {
    struct Pass;

    struct CharLiteral : Expression<IntegerType> {
        char value;

        explicit CharLiteral(char value) noexcept;

        void accept(Pass& visitor) override;
    };
}
