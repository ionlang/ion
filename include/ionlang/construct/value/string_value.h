#pragma once

#include <string>
#include <ionir/misc/helpers.h>
#include <ionir/construct/value.h>

namespace ionlang {
    class Pass;

    class StringValue : public Value<> {
    private:
        std::string value;

    public:
        explicit StringValue(std::string value);

        void accept(Pass &visitor) override;

        std::string getValue() const;

        void setValue(std::string value);
    };
}
