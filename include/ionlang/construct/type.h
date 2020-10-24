#pragma once

#include <ionshared/container/set.h>
#include <ionshared/misc/named.h>
#include "construct.h"

namespace ionlang {
    enum struct TypeQualifier {
        Constant,

        Mutable,

        Reference,

        Pointer
    };

    enum struct TypeKind {
        UserDefined,

        Void,

        Integer,

        String,

        Boolean
    };

    typedef ionshared::Set<TypeQualifier> TypeQualifiers;

    struct Type : Construct, ionshared::Named {
        const TypeKind typeKind;

        std::shared_ptr<TypeQualifiers> qualifiers;

        explicit Type(
            std::string name,
            TypeKind kind = TypeKind::UserDefined,

            std::shared_ptr<TypeQualifiers> qualifiers =
                std::make_shared<TypeQualifiers>()
        ) noexcept;
    };
}
