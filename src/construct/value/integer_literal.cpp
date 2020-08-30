#include <ionlang/passes/pass.h>

namespace ionlang {
    IntegerLiteral::IntegerLiteral(ionshared::Ptr<IntegerType> type, int64_t value)
        : Value(ValueKind::Integer, type), value(value) {
        //
    }

    void IntegerLiteral::accept(Pass &visitor) {
        visitor.visitIntegerValue(this->dynamicCast<IntegerLiteral>());
    }

    int64_t IntegerLiteral::getValue() const {
        if (!this->getType()->getIsSigned()) {
            return std::abs(this->value);
        }

        return this->value;
    }

    void IntegerLiteral::setValue(int64_t value) {
        this->value = value;
    }
}
