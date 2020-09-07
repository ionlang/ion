#pragma once

#include <string>
#include <ionir/construct/value.h>

namespace ionlang {
    class Pass;

    class StringLiteral : public Value<> {
    private:
        std::string value;

    public:
        explicit StringLiteral(std::string value);

        void accept(Pass &visitor) override;

        std::string getValue() const;

        void setValue(std::string value);
    };
}
