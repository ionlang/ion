#pragma once

#include <string>
#include <ionlang/construct/expression.h>

namespace ionlang {
    struct Pass;

    // TODO: Temporary wrong type (should be array of integer types (char = int)).
    struct StringLiteral : Expression<IntegerType> {
        std::string value;

        explicit StringLiteral(std::string value);

        void accept(Pass& visitor) override;
    };
}
