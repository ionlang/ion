#include <ionlang/passes/pass.h>

namespace ionlang {
    Extern::Extern(ionshared::Ptr<Prototype> prototype)
        : Construct(ConstructKind::Extern), prototype(prototype) {
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

    ionshared::Ptr<Prototype> Extern::getPrototype() const noexcept {
        return this->prototype;
    }

    void Extern::setPrototype(ionshared::Ptr<Prototype> prototype) noexcept {
        this->prototype = prototype;
    }
}
