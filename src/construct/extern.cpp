#include <ionlang/passes/pass.h>

namespace ionlang {
    Extern::Extern(ionshared::Ptr<Module> parent, ionshared::Ptr<Prototype> prototype) :
        ConstructWithParent<Module>(std::move(parent), ConstructKind::Extern),
        prototype(std::move(prototype)) {
        //
    }

    void Extern::accept(Pass &visitor) {
        visitor.visitExtern(this->dynamicCast<Extern>());
    }

    Ast Extern::getChildNodes() {
        return {
            this->prototype->nativeCast()
        };
    }
}
