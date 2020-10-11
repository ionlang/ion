#include <ionlang/passes/pass.h>
#include <ionlang/type_system/type_factory.h>

namespace ionlang {
    StringLiteral::StringLiteral(std::string value) :
        Value(ValueKind::String, Resolvable<IntegerType>::make(type_factory::typeString())), // TODO: Awaiting arrays type support (string type).
        value(std::move(value)) {
        //
    }

    void StringLiteral::accept(Pass& visitor) {
        visitor.visitStringLiteral(this->dynamicCast<StringLiteral>());
    }
}
