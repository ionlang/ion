#pragma once

#include <ionshared/container/set.h>
#include <ionshared/misc/named.h>
#include "construct.h"

namespace ionlang {
    enum class TypeQualifier {
        Constant,

        Mutable,

        Reference,

        Pointer
    };

    enum class TypeKind {
        UserDefined,

        Void,

        Integer,

        String,

        Boolean
    };

    typedef ionshared::Set<TypeQualifier> TypeQualifiers;

    struct Type : Construct, ionshared::Named {
        const TypeKind typeKind;

        ionshared::Ptr<TypeQualifiers> qualifiers;

        explicit Type(
            std::string name,
            TypeKind kind = TypeKind::UserDefined,
            ionshared::Ptr<TypeQualifiers> qualifiers = std::make_shared<TypeQualifiers>()
        ) noexcept;
    };
}
