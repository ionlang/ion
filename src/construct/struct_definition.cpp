#include <ionlang/passes/pass.h>

namespace ionlang {
    std::shared_ptr<StructDefExpr> StructDefExpr::make(
        const PtrResolvable<StructType>& type,
        const std::vector<std::shared_ptr<Expression<>>>& values
    ) noexcept {
        std::shared_ptr<StructDefExpr> result =
            std::make_shared<StructDefExpr>(type, values);

        type->setTransitiveParent(result);

        for (const auto& value : values) {
            value->parent = result;
        }

        return result;
    }

    StructDefExpr::StructDefExpr(
        const PtrResolvable<StructType>& type,
        std::vector<std::shared_ptr<Expression<>>> values
    ) :
        Expression<>(
            ExpressionKind::StructDefinition,
            type->staticCast<Resolvable<Type>>()
        ),

        values(std::move(values)) {
        //
    }

    void StructDefExpr::accept(Pass& visitor) {
        visitor.visitStructDefinition(this->dynamicCast<StructDefExpr>());
    }

    Ast StructDefExpr::getChildNodes() {
        Ast children = Construct::convertChildren(this->values);

        children.push_back(this->type);

        return children;
    }
}
