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

    class Expression : public Value<> {
    private:
        ExpressionKind kind;

    public:
        explicit Expression(ExpressionKind kind, ionshared::Ptr<Type> type);

        void accept(Pass &visitor) override;

        ExpressionKind getExpressionKind() const noexcept;
    };
}
