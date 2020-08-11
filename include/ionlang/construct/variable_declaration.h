#pragma once

#include <ionshared/misc/helpers.h>
#include <string>
#include "construct.h"

namespace ionlang {
    class Pass;

    class VariableDeclaration : public Construct {
    private:
        std::string id;

        ionshared::Ptr<Construct> value;

    public:
        explicit VariableDeclaration(std::string id, ionshared::Ptr<Construct> value);

        void accept(Pass &visitor) override;

        std::string getId() const noexcept;

        void setId(std::string value) noexcept;

        ionshared::Ptr<Construct> getValue() const noexcept;

        void setValue(ionshared::Ptr<Construct> value) noexcept;
    };
}
