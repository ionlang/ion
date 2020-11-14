#pragma once

#include <ionlang/construct/type.h>

namespace ionlang {
    struct Pass;

    struct BooleanType : Type {
        explicit BooleanType(
            std::shared_ptr<TypeQualifierSet> qualifiers =
                std::make_shared<TypeQualifierSet>()
        );

        void accept(Pass& pass) override;
    };
}
