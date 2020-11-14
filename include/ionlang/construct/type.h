#pragma once

#include <ionshared/container/set.h>
#include <ionshared/misc/named.h>
#include "construct.h"

namespace ionlang {
    enum struct TypeQualifier {
        Constant,

        Mutable,

        Reference,

        Pointer,

        Nullable
    };

    enum struct TypeKind : uint32_t {
        Struct,

        Void,

        Integer,

        String,

        Boolean
    };

    typedef ionshared::Set<TypeQualifier> TypeQualifierSet;

    struct Type : Construct {
        const std::string typeName;

        const TypeKind typeKind;

        std::shared_ptr<TypeQualifierSet> qualifiers;

        Type(
            std::string name,
            TypeKind kind,

            std::shared_ptr<TypeQualifierSet> qualifiers =
                std::make_shared<TypeQualifierSet>()
        ) noexcept;
    };
}
