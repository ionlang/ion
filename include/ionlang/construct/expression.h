#pragma once

#include <ionlang/construct/pseudo/resolvable.h>

namespace ionlang {
    struct Pass;

    struct Type;

    enum struct ExpressionKind {
        Value,

        Call,

        Operation
    };

    struct Expression : Construct {
        const ExpressionKind expressionKind;

        PtrResolvable<Type> type;

        Expression(ExpressionKind kind, PtrResolvable<Type> type);
    };
}
