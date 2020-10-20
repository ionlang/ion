#include <ionlang/passes/pass.h>

namespace ionlang {
    Function::Function(
        std::shared_ptr<Module> parent,
        std::shared_ptr<Prototype> prototype,
        std::shared_ptr<Block> body
    ) :
        ConstructWithParent<Module>(std::move(parent), ConstructKind::Function),
        prototype(std::move(prototype)),
        body(std::move(body)) {
        //
    }

    void Function::accept(Pass &visitor) {
        visitor.visitFunction(this->dynamicCast<Function>());
    }

    Ast Function::getChildNodes() {
        return {
            this->prototype->nativeCast(),
            this->body->nativeCast()
        };
    }
}
