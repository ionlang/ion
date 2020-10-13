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

        ionshared::Ptr<Expression<>> leftSideValue;

        ionshared::OptPtr<Expression<>> rightSideValue;
    };

    struct OperationExpr : Expression<> {
        const IntrinsicOperatorKind operation;

        ionshared::Ptr<Expression<>> leftSideValue;

        ionshared::OptPtr<Expression<>> rightSideValue;

        explicit OperationExpr(const OperationExprOpts& opts);

        void accept(Pass& visitor) override;

        [[nodiscard]] Ast getChildNodes() override;
    };
}
