#pragma once

#include <ionlang/construct/pseudo/resolvable.h>
#include "value.h"
#include "type.h"

namespace ionlang {
    class Pass;

    enum class ExpressionKind {
        Call,

        UnaryOperation,

        BinaryOperation
    };

    struct Expression : Value<> {
        const ExpressionKind expressionKind;

        Expression(ExpressionKind kind, PtrResolvable<Type> typeRef);

        void accept(Pass& visitor) override;
    };
}
