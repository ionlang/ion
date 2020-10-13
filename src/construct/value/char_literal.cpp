#include <ionlang/type_system/type_factory.h>
#include <ionlang/passes/pass.h>

namespace ionlang {
    CharLiteral::CharLiteral(char value) noexcept :
        Expression<IntegerType>(ExpressionKind::CharLiteral, type_factory::typeChar()),
        value(value) {
        //
    }

    void CharLiteral::accept(Pass& visitor) {
        visitor.visitCharLiteral(this->dynamicCast<CharLiteral>());
    }
}
