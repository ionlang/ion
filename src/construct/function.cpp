#include <ionlang/passes/pass.h>

namespace ionlang {
    std::shared_ptr<Function> Function::make(
        const std::shared_ptr<Prototype>& prototype,
        const std::shared_ptr<Block>& body
    ) noexcept {
        std::shared_ptr<Function> result =
            std::make_shared<Function>(prototype, body);

        prototype->setParent(result);
        body->setParent(result);

        return result;
    }

    Function::Function(
        std::shared_ptr<Prototype> prototype,
        std::shared_ptr<Block> body
    ) :
        ConstructWithParent<Module, Construct, ConstructKind>(ConstructKind::Function),
        prototype(std::move(prototype)),
        body(std::move(body)) {
        //
    }

    void Function::accept(Pass& visitor) {
        visitor.visitFunction(this->dynamicCast<Function>());
    }

    Ast Function::getChildNodes() {
        return {
            this->prototype,
            this->body
        };
    }
}
