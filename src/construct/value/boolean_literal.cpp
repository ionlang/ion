#include <ionlang/passes/pass.h>

namespace ionlang {
    BooleanLiteral::BooleanLiteral(bool value) :
        Value(ValueKind::Boolean, std::make_shared<BooleanType>()),
        value(value) {
        //
    }

    void BooleanLiteral::accept(Pass &visitor) {
        visitor.visitBooleanLiteral(this->dynamicCast<BooleanLiteral>());
    }
}
