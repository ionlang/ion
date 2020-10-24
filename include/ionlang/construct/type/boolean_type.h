#pragma once

#include <ionlang/construct/type.h>

namespace ionlang {
    struct Pass;

    struct BooleanType : Type {
        explicit BooleanType(
            std::shared_ptr<TypeQualifiers> qualifiers =
                std::make_shared<TypeQualifiers>()
        );

        void accept(Pass& pass) override;
    };
}
