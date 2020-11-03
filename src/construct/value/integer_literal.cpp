#include <ionlang/passes/pass.h>

namespace ionlang {
    std::shared_ptr<IntegerLiteral> IntegerLiteral::make(
        const std::shared_ptr<IntegerType>& type,
        int64_t value
    ) noexcept {
        std::shared_ptr<IntegerLiteral> result =
            std::make_shared<IntegerLiteral>(type, value);

        type->parent = result;

        return result;
    }

    IntegerLiteral::IntegerLiteral(std::shared_ptr<IntegerType> type, int64_t value) noexcept :
        Expression<IntegerType>(ExpressionKind::IntegerLiteral, std::move(type)),
        value(value) {
        //
    }

    void IntegerLiteral::accept(Pass& visitor) {
        visitor.visitIntegerLiteral(this->dynamicCast<IntegerLiteral>());
    }
}
