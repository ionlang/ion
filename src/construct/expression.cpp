#include <ionlang/passes/pass.h>

namespace ionlang {
    Expression::Expression(ExpressionKind kind, PtrResolvable<Type> typeRef) :
        Value<>(ValueKind::Expression, std::move(typeRef)),
        expressionKind(kind) {
        //
    }

    void Expression::accept(Pass& visitor) {
        // TODO: Verify this works.
        visitor.visitExpression(this->dynamicCast<Expression>());
    }
}
