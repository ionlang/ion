#pragma once

#include <ionlang/construct/value.h>

namespace ionlang {
    class Pass;

    struct CharLiteral : Value<> {
        char value;

        explicit CharLiteral(char value);

        void accept(Pass &visitor) override;
    };
}
