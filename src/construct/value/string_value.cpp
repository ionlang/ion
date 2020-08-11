#include <ionlang/passes/pass.h>

namespace ionlang {
    StringValue::StringValue(std::string value)
        : Value(ValueKind::String, TypeFactory::typeString()), value(value) {
        //
    }

    void StringValue::accept(Pass &visitor) {
        visitor.visitStringValue(this->dynamicCast<StringValue>());
    }

    std::string StringValue::getValue() const {
        return this->value;
    }

    void StringValue::setValue(std::string value) {
        this->value = value;
    }
}
