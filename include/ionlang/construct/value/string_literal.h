#pragma once

#include <string>
#include <ionir/construct/value.h>

namespace ionlang {
    class Pass;

    // TODO: Temporary wrong type (should be array of integer types (char = int)).
    struct StringLiteral : Value<IntegerType> {
        std::string value;

        explicit StringLiteral(std::string value);

        void accept(Pass& visitor) override;
    };
}
