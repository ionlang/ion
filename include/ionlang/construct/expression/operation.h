#pragma once

#include <ionlang/construct/expression.h>

namespace ionlang {
    class Pass;

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

        ionshared::Ptr<Expression> leftSide;

        ionshared::OptPtr<Expression> rightSide;
    };

    struct OperationExpr : Expression {
        IntrinsicOperatorKind operation;

        ionshared::Ptr<Expression> leftSide;

        ionshared::OptPtr<Expression> rightSide;

        explicit OperationExpr(const OperationExprOpts& opts);
    };
}
