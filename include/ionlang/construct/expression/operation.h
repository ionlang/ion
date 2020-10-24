#pragma once

#include <ionlang/construct/expression.h>

namespace ionlang {
    struct Pass;

    enum struct IntrinsicOperatorKind {
        Equal,

        NotEqual,

        Addition,

        Subtraction,

        Multiplication,

        Division,

        Modulo,

        LessThan,

        GreaterThan,

        LessThanOrEqualTo,

        GreaterThanOrEqualTo,

        And,

        Or,

        Not
    };

    struct OperationExpr : Expression<> {
        static std::shared_ptr<OperationExpr> make(
            const PtrResolvable<Type>& type,
            IntrinsicOperatorKind operation,
            const std::shared_ptr<Expression<>>& leftSideValue,
            ionshared::OptPtr<Expression<>> rightSideValue
        ) noexcept;

        const IntrinsicOperatorKind operation;

        std::shared_ptr<Expression<>> leftSideValue;

        ionshared::OptPtr<Expression<>> rightSideValue;

        OperationExpr(
            PtrResolvable<Type> type,
            IntrinsicOperatorKind operation,
            std::shared_ptr<Expression<>> leftSideValue,
            ionshared::OptPtr<Expression<>> rightSideValue
        ) noexcept;

        void accept(Pass& visitor) override;

        [[nodiscard]] Ast getChildNodes() override;

        [[nodiscard]] bool isBinary() const noexcept;
    };
}
