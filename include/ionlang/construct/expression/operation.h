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

    struct OperationExprOpts {
        PtrResolvable<Type> type;

        IntrinsicOperatorKind operation;

        std::shared_ptr<Expression<>> leftSideValue;

        ionshared::OptPtr<Expression<>> rightSideValue;
    };

    struct OperationExpr : Expression<> {
        const IntrinsicOperatorKind operation;

        std::shared_ptr<Expression<>> leftSideValue;

        ionshared::OptPtr<Expression<>> rightSideValue;

        explicit OperationExpr(const OperationExprOpts& opts) noexcept;

        void accept(Pass& visitor) override;

        [[nodiscard]] Ast getChildNodes() override;

        [[nodiscard]] bool isBinary() const noexcept;
    };
}
