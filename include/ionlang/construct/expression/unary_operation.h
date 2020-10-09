#pragma once

#include <ionlang/construct/expression.h>

namespace ionlang {
    class Pass;

    enum class IntrinsicOperatorKind {
        Addition,

        Subtraction,

        Multiplication,

        Division,

        Exponent,

        Modulo,

        LessThan,

        GreaterThan
    };

    struct UnaryOperation : Expression {
        IntrinsicOperatorKind operation;

        ionshared::Ptr<Construct> value;

        UnaryOperation(
            ionshared::Ptr<Type> type,
            IntrinsicOperatorKind operation,
            ionshared::Ptr<Construct> value
        );
    };
}
