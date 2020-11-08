#include <ionlang/passes/pass.h>

namespace ionlang {
    std::shared_ptr<Cast> Cast::make(
        const std::shared_ptr<Type>& type,
        const std::shared_ptr<Expression<>>& value
    ) noexcept {
        std::shared_ptr<Cast> result =
            std::make_shared<Cast>(type, value);

        type->parent = result;
        value->parent = result;

        return result;
    }

    Cast::Cast(
        std::shared_ptr<Type> type,
        std::shared_ptr<Expression<>> value
    ) noexcept :
        type(std::move(type)),
        value(std::move(value)) {
        //
    }

    void Cast::accept(Pass& visitor) {
        visitor.visitCast(this->dynamicCast<Cast>());
    }

    Ast Cast::getChildNodes() {
        return {
            this->type,
            this->value
        };
    }
}
