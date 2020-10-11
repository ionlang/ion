#pragma once

#include <ionlang/construct/pseudo/resolvable.h>
#include <ionlang/construct/type.h>

namespace ionlang {
    enum class ValueKind {
        Integer,

        Character,

        String,

        Boolean,

        Expression
    };

    template<typename T = Type>
    struct Value : Construct {
        const ValueKind valueKind;

        PtrResolvable<T> type;

        Value(ValueKind kind, PtrResolvable<T> type) noexcept :
            Construct(ConstructKind::Value),
            valueKind(kind),
            type(type) {
            //
        }
    };
}
