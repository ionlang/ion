#pragma once

#include <ionlang/construct/type.h>

namespace ionlang {
    class Pass;

    struct UserDefinedType : Type {
        explicit UserDefinedType(
            ionshared::Ptr<TypeQualifiers> qualifiers =
            std::make_shared<TypeQualifiers>()
        );

        void accept(Pass &pass) override;
    };
}
