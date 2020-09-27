#include <ionlang/passes/pass.h>
#include <ionlang/type_system/type_factory.h>

namespace ionlang {
    StringLiteral::StringLiteral(std::string value)
        // TODO: Awaiting arrays type support (string type).
        : Value(ValueKind::String, type_factory::typeString()), value(std::move(value)) {
        //
    }

    void StringLiteral::accept(Pass &visitor) {
        visitor.visitStringLiteral(this->dynamicCast<StringLiteral>());
    }
}
