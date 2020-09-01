#pragma once

#include <exception>
#include <ionlang/construct/type.h>

namespace ionlang::type_util {
    bool isAtomicTypesCompatible(
        TypeKind typeKindA,
        TypeKind typeKindB
    );

    bool isSameType(
        const ionshared::Ptr<Type> &typeA,
        const ionshared::Ptr<Type> &typeB
    );
}
