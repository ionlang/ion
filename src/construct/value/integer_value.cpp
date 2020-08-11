#include <ionlang/passes/pass.h>

namespace ionlang {
    IntegerValue::IntegerValue(ionshared::Ptr<IntegerType> type, int64_t value)
        : Value(ValueKind::Integer, type), value(value) {
        //
    }

    void IntegerValue::accept(Pass &visitor) {
        visitor.visitIntegerValue(this->dynamicCast<IntegerValue>());
    }

    int64_t IntegerValue::getValue() const {
        if (!this->getType()->getIsSigned()) {
            return std::abs(this->value);
        }

        return this->value;
    }

    void IntegerValue::setValue(int64_t value) {
        this->value = value;
    }
}
