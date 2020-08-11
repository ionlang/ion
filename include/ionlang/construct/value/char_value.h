#pragma once

#include <ionlang/construct/value.h>

namespace ionlang {
    class Pass;

    class CharValue : public Value<> {
    private:
        char value;

    public:
        explicit CharValue(char value);

        void accept(Pass &visitor) override;

        char getValue() const;

        void setValue(const char value);
    };
}
