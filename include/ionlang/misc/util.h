#pragma once

#include <ionlang/construct/type/integer_type.h>
#include <ionlang/construct/statement.h>

namespace ionlang {
    class Util {
    public:
        static std::string resolveIntegerKindName(IntegerKind kind);

        static TypeKind resolveTypeKind(const std::string &id);

        static std::optional<IntegerKind> calculateIntegerKindFromBitLength(uint32_t bitLength) noexcept;

        static std::optional<std::string> findConstructId(const ionshared::Ptr<Construct> &construct);

        static std::optional<std::string> findStatementId(const ionshared::Ptr<Statement> &statement) noexcept;
    };
}
