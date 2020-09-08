#pragma once

#include <ionlang/construct/type.h>

namespace ionlang {
    class Pass;

    class BooleanType : public Type {
    public:
        explicit BooleanType(
            ionshared::Ptr<TypeQualifiers> qualifiers = std::make_shared<TypeQualifiers>()
        );

        void accept(Pass &pass) override;
    };
}
