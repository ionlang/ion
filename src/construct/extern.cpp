#include <ionlang/passes/pass.h>

namespace ionlang {
    std::shared_ptr<Extern> Extern::make(
        const std::shared_ptr<Prototype>& prototype
    ) noexcept {
        std::shared_ptr<Extern> result =
            std::make_shared<Extern>(prototype);

        prototype->setParent(result);

        return result;
    }

    Extern::Extern(std::shared_ptr<Prototype> prototype) :
        ConstructWithParent<Module, Construct, ConstructKind>(ConstructKind::Extern),
        prototype(std::move(prototype)) {
        //
    }

    void Extern::accept(Pass& visitor) {
        visitor.visitExtern(this->dynamicCast<Extern>());
    }

    Ast Extern::getChildNodes() {
        return {
            this->prototype
        };
    }
}
