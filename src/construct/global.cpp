#include <ionlang/passes/pass.h>

namespace ionlang {
    std::shared_ptr<Global> Global::make(
        const PtrResolvable<Type>& type,
        const std::string& name,
        ionshared::OptPtr<Expression<>> value
    ) noexcept {
        std::shared_ptr<Global> result =
            std::make_shared<Global>(type, name, value);

        type->parent = result;

        if (ionshared::util::hasValue(value)) {
            value->get()->parent = result;
        }

        return result;
    }

    Global::Global(
        const PtrResolvable<Type>& type,
        std::string name,
        ionshared::OptPtr<Expression<>> value
    ) :
        ConstructWithParent<Module, Construct, ConstructKind>(ConstructKind::Global),
        ionshared::Named{std::move(name)},
        type(std::move(type)),
        value(std::move(value)) {
        //
    }

    void Global::accept(Pass& visitor) {
        visitor.visitGlobal(this->dynamicCast<Global>());
    }

    Ast Global::getChildNodes() {
        Ast children{this->type};

        if (ionshared::util::hasValue(this->value)) {
            children.push_back(this->value->get()->nativeCast());
        }

        return children;
    }
}
