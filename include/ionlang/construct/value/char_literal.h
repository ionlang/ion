#pragma once

#include <ionlang/construct/value.h>

namespace ionlang {
    class Pass;

    class CharLiteral : public Value<> {
    private:
        char value;

    public:
        explicit CharLiteral(char value);

        void accept(Pass &visitor) override;

        [[nodiscard]] char getValue() const noexcept;

        void setValue(char value) noexcept;
    };
}
