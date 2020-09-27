#include <ionlang/passes/pass.h>

namespace ionlang {
    Expression::Expression(ExpressionKind kind, ionshared::Ptr<Type> type) :
        Value<>(ValueKind::Expression, std::move(type)),
        expressionKind(kind) {
        //
    }

    void Expression::accept(Pass &visitor) {
        // TODO: Verify this works.
        visitor.visitExpression(this->dynamicCast<Expression>());
    }
}
