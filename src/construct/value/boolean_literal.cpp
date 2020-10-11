#include <ionlang/type_system/type_factory.h>
#include <ionlang/passes/pass.h>

namespace ionlang {
    BooleanLiteral::BooleanLiteral(bool value) :
        Value(ValueKind::Boolean, Resolvable<BooleanType>::make(type_factory::typeBoolean())),
        value(value) {
        //
    }

    void BooleanLiteral::accept(Pass& visitor) {
        visitor.visitBooleanLiteral(this->dynamicCast<BooleanLiteral>());
    }
}
