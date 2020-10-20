#include <ionlang/passes/pass.h>

namespace ionlang {
    Global::Global(
        std::shared_ptr<Module> parent,
        std::shared_ptr<Type> type,
        std::string name,
        ionshared::OptPtr<Expression<>> value
    ) :
        ConstructWithParent<Module>(std::move(parent), ConstructKind::Global),
        ionshared::Named{std::move(name)},
        type(std::move(type)),
        value(std::move(value)) {
        //
    }

    void Global::accept(Pass& visitor) {
        visitor.visitGlobal(this->dynamicCast<Global>());
    }

    Ast Global::getChildNodes() {
        Ast children = {
            this->type->nativeCast()
        };

        auto e = this->type->nativeCast();

        if (this->value.has_value()) {
            children.push_back(this->value->get()->nativeCast());
        }

        return children;
    }
}
