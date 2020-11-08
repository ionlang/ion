#pragma once

#include <ionlang/construct/type.h>
#include <ionlang/construct/pseudo/resolvable.h>

namespace ionlang {
    struct Pass;

    enum struct ExpressionKind : uint32_t {
        Call,

        Operation,

        VariableReference,

        BooleanLiteral,

        CharLiteral,

        IntegerLiteral,

        StringLiteral,

        StructDefinition,

        Cast
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

        Expression(ExpressionKind kind, std::shared_ptr<T> type) noexcept :
            Expression(kind, Resolvable<T>::make(std::move(type))) {
            //
        }

        std::shared_ptr<Expression<>> flattenExpression() {
            return this->staticCast<Expression<>>();
        }
    };
}
