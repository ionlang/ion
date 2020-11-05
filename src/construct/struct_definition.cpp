#include <ionlang/passes/pass.h>

namespace ionlang {
    std::shared_ptr<StructDefinition> StructDefinition::make(
        const PtrResolvable<StructType>& type,
        const std::vector<std::shared_ptr<Expression<>>>& values
    ) noexcept {
        std::shared_ptr<StructDefinition> result =
            std::make_shared<StructDefinition>(type, values);

        type->setTransitiveParent(result);

        for (const auto& value : values) {
            value->parent = result;
        }

        return result;
    }

    StructDefinition::StructDefinition(
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

    void StructDefinition::accept(Pass& visitor) {
        visitor.visitStructDefinition(this->dynamicCast<StructDefinition>());
    }

    Ast StructDefinition::getChildNodes() {
        Ast children = Construct::convertChildren(this->values);

        children.push_back(this->type);

        return children;
    }
}
