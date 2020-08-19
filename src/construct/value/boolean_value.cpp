#include <ionlang/passes/pass.h>

namespace ionlang {
    BooleanValue::BooleanValue(bool value)
        : Value(ValueKind::Boolean, std::make_shared<BooleanType>()), value(value) {
        //
    }

    void BooleanValue::accept(Pass &visitor) {
        visitor.visitBooleanValue(this->dynamicCast<BooleanValue>());
    }

    bool BooleanValue::getValue() const {
        return this->value;
    }

    void BooleanValue::setValue(bool value) {
        this->value = value;
    }
}
