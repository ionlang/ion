#pragma once

#include <ionlang/construct/type/integer_type.h>
#include <ionlang/construct/type/boolean_type.h>
#include <ionlang/construct/type/void_type.h>

namespace ionlang::type_factory {
    [[nodiscard]] std::shared_ptr<IntegerType> typeInteger(
        IntegerKind integerKind,
        bool isSigned = true
    );

    [[nodiscard]] std::shared_ptr<IntegerType> typeInteger8(
        bool isSigned = true
    );

    [[nodiscard]] std::shared_ptr<IntegerType> typeInteger16(
        bool isSigned = true
    );

    [[nodiscard]] std::shared_ptr<IntegerType> typeInteger32(
        bool isSigned = true
    );

    [[nodiscard]] std::shared_ptr<IntegerType> typeInteger64(
        bool isSigned = true
    );

    [[nodiscard]] std::shared_ptr<IntegerType> typeInteger128(
        bool isSigned = true
    );

    [[nodiscard]] std::shared_ptr<BooleanType> typeBoolean();

    [[nodiscard]] std::shared_ptr<IntegerType> typeChar();

    // TODO: return type.
    [[nodiscard]] std::shared_ptr<IntegerType> typeString();

    [[nodiscard]] std::shared_ptr<VoidType> typeVoid();
}
