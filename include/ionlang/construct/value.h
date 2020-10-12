#pragma once

#include <ionlang/construct/pseudo/resolvable.h>
#include "expression.h"

namespace ionlang {
    enum struct ValueKind {
        Integer,

        Character,

        String,

        Boolean,

        Expression
    };

    // TODO: What if Type is never included?
    struct Type;

    template<typename T = Type>
    struct Value : Expression {
        const ValueKind valueKind;

        Value(ValueKind kind, PtrResolvable<T> type) noexcept :
            Expression(ExpressionKind::Value, type),
            valueKind(kind) {
            //
        }
    };
}
