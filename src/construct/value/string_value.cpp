#include <ionlang/passes/pass.h>
#include <ionlang/misc/type_factory.h>

namespace ionlang {
    StringValue::StringValue(std::string value)
        // TODO: Awaiting arrays type support (string type).
        : Value(ValueKind::String, TypeFactory::typeString()), value(std::move(value)) {
        //
    }

    void StringValue::accept(Pass &visitor) {
        visitor.visitStringValue(this->dynamicCast<StringValue>());
    }

    std::string StringValue::getValue() const {
        return this->value;
    }

    void StringValue::setValue(std::string value) {
        this->value = std::move(value);
    }
}
