#pragma once

#include <ionlang/construct/value.h>

namespace ionlang {
    class Pass;

    class BooleanValue : public Value<> {
    private:
        bool value;

    public:
        explicit BooleanValue(bool value);

        void accept(Pass &visitor) override;

        bool getValue() const;

        void setValue(bool value);
    };
}
