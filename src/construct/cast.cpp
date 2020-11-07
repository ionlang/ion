#include <ionlang/passes/pass.h>

namespace ionlang {
    std::shared_ptr<Cast> Cast::make(
        const std::shared_ptr<Type>& type
    ) noexcept {
        std::shared_ptr<Cast> result =
            std::make_shared<Cast>(type);

        type->parent = result;

        return result;
    }

    Cast::Cast(std::shared_ptr<Type> type) :
        type(std::move(type)) {
        //
    }

    void Cast::accept(Pass& visitor) {
        visitor.visitCast(this->dynamicCast<Cast>());
    }

    Ast Cast::getChildNodes() {
        return {this->type};
    }
}
