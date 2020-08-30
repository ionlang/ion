#pragma once

#include <ionlang/construct/expression/unary_operation.h>
#include <ionlang/construct/type/integer_type.h>
#include <ionlang/construct/statement.h>
#include <ionlang/lexical/token_kind.h>

namespace ionlang::util {
    [[nodiscard]] std::string resolveIntegerKindName(IntegerKind kind);

    [[nodiscard]] TypeKind resolveTypeKind(const std::string &id);

    [[nodiscard]] std::optional<IntegerKind> calculateIntegerKindFromBitLength(uint32_t bitLength) noexcept;

    [[nodiscard]] std::optional<std::string> findConstructId(const ionshared::Ptr<Construct> &construct);

    [[nodiscard]] std::optional<std::string> findStatementId(const ionshared::Ptr<Statement> &statement) noexcept;

    [[nodiscard]] std::optional<Operator> findOperator(TokenKind tokenKind);
}
