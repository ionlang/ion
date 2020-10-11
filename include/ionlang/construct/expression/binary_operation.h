#pragma once

#include <ionlang/construct/expression.h>
#include "unary_operation.h"

namespace ionlang {
    class Pass;

    struct BinaryOperationOpts {
        PtrResolvable<Type> type;

        IntrinsicOperatorKind operation;

        ionshared::Ptr<Expression> leftSide;

        ionshared::Ptr<Expression> rightSide;
    };

    struct BinaryOperation : Expression {
        IntrinsicOperatorKind operation;

        ionshared::Ptr<Expression> leftSide;

        ionshared::Ptr<Expression> rightSide;

        explicit BinaryOperation(const BinaryOperationOpts& opts);
    };
}
