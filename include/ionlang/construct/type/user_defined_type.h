#pragma once

#include <ionlang/construct/type.h>

namespace ionlang {
    class Pass;

    struct UserDefinedType : Type {
        explicit UserDefinedType(
            std::string name,

            std::shared_ptr<TypeQualifiers> qualifiers =
                std::make_shared<TypeQualifiers>()
        ) noexcept;

        void accept(Pass& pass) override;
    };
}
