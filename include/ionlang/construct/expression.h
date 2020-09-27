#pragma once

#include "value.h"
#include "type.h"

namespace ionlang {
    class Pass;

    enum class ExpressionKind {
        Call,

        UnaryOperation,

        BinaryOperation
    };

    struct Expression : public Value<> {
        const ExpressionKind expressionKind;

        Expression(ExpressionKind kind, ionshared::Ptr<Type> type);

        void accept(Pass &visitor) override;
    };
}
