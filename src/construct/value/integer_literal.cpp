#include <ionlang/passes/pass.h>

namespace ionlang {
    IntegerLiteral::IntegerLiteral(std::shared_ptr<IntegerType> type, int64_t value) :
        Expression<IntegerType>(ExpressionKind::IntegerLiteral, std::move(type)),
        value(value) {
        //
    }

    void IntegerLiteral::accept(Pass& visitor) {
        visitor.visitIntegerLiteral(this->dynamicCast<IntegerLiteral>());
    }
}
