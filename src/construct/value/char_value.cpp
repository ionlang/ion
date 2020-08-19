#include <ionlang/passes/pass.h>

namespace ionlang {
    CharValue::CharValue(char value)
        : Value(ValueKind::Character, std::make_shared<IntegerType>(IntegerKind::Int8, false)), value(value) {
        //
    }

    void CharValue::accept(Pass &visitor) {
        visitor.visitCharValue(this->dynamicCast<CharValue>());
    }

    char CharValue::getValue() const {
        return this->value;
    }

    void CharValue::setValue(const char value) {
        this->value = value;
    }
}
