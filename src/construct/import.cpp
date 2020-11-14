#include <ionlang/passes/pass.h>

namespace ionlang {
    std::shared_ptr<Import> Import::make(
        const std::shared_ptr<Identifier>& id
    ) noexcept {
        std::shared_ptr<Import> result =
            std::make_shared<Import>(id);

        id->setParent(result);

        return result;
    }

    Import::Import(std::shared_ptr<Identifier> id) :
        Construct(ConstructKind::Import),
        id(std::move(id)) {
        //
    }

    void Import::accept(Pass& visitor) {
        visitor.visitImport(this->dynamicCast<Import>());
    }

    Ast Import::getChildNodes() {
        return {this->id};
    }
}
