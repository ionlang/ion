#include <ionlang/passes/pass.h>

namespace ionlang {
    std::shared_ptr<StructDefinition> StructDefinition::make(
        const PtrResolvable<Struct>& declaration,
        std::vector<std::shared_ptr<Expression<>>> values
    ) noexcept {
        std::shared_ptr<StructDefinition> result =
            std::make_shared<StructDefinition>(declaration, values);

        for (const auto& value : values) {
            value->parent = result;
        }

        return result;
    }

    StructDefinition::StructDefinition(
        const PtrResolvable<Struct>& declaration,
        std::vector<std::shared_ptr<Expression<>>> values
    ) noexcept :
        Expression<>(
            ExpressionKind::StructDefinition,

            // TODO: CRITICAL: How do we know declaration.name is not std::nullopt?
            std::make_shared<UserDefinedType>(*declaration->name)
        ),

        declaration(declaration),
        values(std::move(values)) {
        //
    }

    void StructDefinition::accept(Pass& visitor) {
        visitor.visitStructDefinition(this->dynamicCast<StructDefinition>());
    }

    Ast StructDefinition::getChildNodes() {
        Ast children = Construct::convertChildren(this->values);

        // TODO: Declaration isn't technically a child, instead a resolvable. Maybe need a different getResolvables() method?
        children.push_back(this->declaration);

        return children;
    }
}
