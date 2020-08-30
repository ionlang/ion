#include <ionlang/passes/pass.h>
#include <ionlang/misc/type_factory.h>

namespace ionlang {
    StringLiteral::StringLiteral(std::string value)
        // TODO: Awaiting arrays type support (string type).
        : Value(ValueKind::String, type_factory::typeString()), value(std::move(value)) {
        //
    }

    void StringLiteral::accept(Pass &visitor) {
        visitor.visitStringValue(this->dynamicCast<StringLiteral>());
    }

    std::string StringLiteral::getValue() const {
        return this->value;
    }

    void StringLiteral::setValue(std::string value) {
        this->value = std::move(value);
    }
}
