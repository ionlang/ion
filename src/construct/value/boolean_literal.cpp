#include <ionlang/passes/pass.h>

namespace ionlang {
    BooleanLiteral::BooleanLiteral(bool value)
        : Value(ValueKind::Boolean, std::make_shared<BooleanType>()), value(value) {
        //
    }

    void BooleanLiteral::accept(Pass &visitor) {
        visitor.visitBooleanValue(this->dynamicCast<BooleanLiteral>());
    }

    bool BooleanLiteral::getValue() const {
        return this->value;
    }

    void BooleanLiteral::setValue(bool value) {
        this->value = value;
    }
}
