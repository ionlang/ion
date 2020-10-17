#pragma once

#include <ionlang/construct/pseudo/resolvable.h>

namespace ionlang {
    struct Pass;

    struct Type;

    enum struct ExpressionKind {
        Value,

        Call,

        Operation,

        VariableReference,

        BooleanLiteral,

        CharLiteral,

        IntegerLiteral,

        StringLiteral,

        StructDefinition
    };

    template<typename T = Type>
        requires std::derived_from<T, Type>
    struct Expression : Construct {
        const ExpressionKind expressionKind;

        PtrResolvable<T> type;

        Expression(ExpressionKind kind, PtrResolvable<T> type) noexcept :
            Construct(ConstructKind::Expression),
            expressionKind(kind),
            type(std::move(type)) {
            //
        }

        Expression(ExpressionKind kind, ionshared::Ptr<T> type) noexcept :
            Expression(kind, Resolvable<T>::make(std::move(type))) {
            //
        }

        ionshared::Ptr<Expression<>> flatten() {
            return this->staticCast<Expression<>>();
        }
    };
}
