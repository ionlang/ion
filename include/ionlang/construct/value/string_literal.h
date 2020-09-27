#pragma once

#include <string>
#include <ionir/construct/value.h>

namespace ionlang {
    class Pass;

    struct StringLiteral : Value<> {
        std::string value;

        explicit StringLiteral(std::string value);

        void accept(Pass &visitor) override;
    };
}
