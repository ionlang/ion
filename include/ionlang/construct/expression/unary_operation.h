#pragma once

#include <ionlang/construct/expression.h>

namespace ionlang {
    class Pass;

    enum struct IntrinsicOperatorKind {
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
            PtrResolvable<Type> type,
            IntrinsicOperatorKind operation,
            ionshared::Ptr<Construct> value
        );
    };
}
