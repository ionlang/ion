#pragma once

#include <ionlang/construct/type/integer_type.h>

namespace ionlang {
    class Util {
    public:
        static std::string resolveIntegerKindName(IntegerKind kind);

        static TypeKind resolveTypeKind(const std::string &id);

        static std::optional<IntegerKind> calculateIntegerKindFromBitLength(uint32_t bitLength) noexcept;
    };
}
