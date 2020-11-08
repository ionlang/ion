#include <ionlang/passes/pass.h>

namespace ionlang {
    std::shared_ptr<CastExpr> CastExpr::make(
        const std::shared_ptr<Resolvable<Type>>& type,
        const std::shared_ptr<Expression<>>& value
    ) noexcept {
        std::shared_ptr<CastExpr> result =
            std::make_shared<CastExpr>(type, value);

        type->setTransitiveParent(result);
        value->parent = result;

        return result;
    }

    CastExpr::CastExpr(
        std::shared_ptr<Resolvable<Type>> type,
        std::shared_ptr<Expression<>> value
    ) noexcept :
        Expression<>(ExpressionKind::CastExpr),
        type(std::move(type)),
        value(std::move(value)) {
        //
    }

    void CastExpr::accept(Pass& visitor) {
        visitor.visitCastExpr(this->dynamicCast<CastExpr>());
    }

    Ast CastExpr::getChildNodes() {
        return {
            this->type,
            this->value
        };
    }
}
