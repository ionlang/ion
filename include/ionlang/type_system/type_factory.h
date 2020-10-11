#pragma once

#include <ionlang/construct/type/integer_type.h>
#include <ionlang/construct/type/boolean_type.h>
#include <ionlang/construct/type/void_type.h>

namespace ionlang::type_factory {
    [[nodiscard]] ionshared::Ptr<IntegerType> typeInteger(
        IntegerKind integerKind,
        bool isSigned = true
    );

    [[nodiscard]] ionshared::Ptr<IntegerType> typeInteger8(
        bool isSigned = true
    );

    [[nodiscard]] ionshared::Ptr<IntegerType> typeInteger16(
        bool isSigned = true
    );

    [[nodiscard]] ionshared::Ptr<IntegerType> typeInteger32(
        bool isSigned = true
    );

    [[nodiscard]] ionshared::Ptr<IntegerType> typeInteger64(
        bool isSigned = true
    );

    [[nodiscard]] ionshared::Ptr<IntegerType> typeInteger128(
        bool isSigned = true
    );

    [[nodiscard]] ionshared::Ptr<BooleanType> typeBoolean();

    [[nodiscard]] ionshared::Ptr<IntegerType> typeChar();

    // TODO: return type.
    [[nodiscard]] ionshared::Ptr<IntegerType> typeString();

    [[nodiscard]] ionshared::Ptr<VoidType> typeVoid();
}
