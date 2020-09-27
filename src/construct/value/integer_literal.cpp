#include <ionlang/passes/pass.h>

namespace ionlang {
    IntegerLiteral::IntegerLiteral(ionshared::Ptr<IntegerType> type, int64_t value) :
        Value(ValueKind::Integer, std::move(type)), value(value) {
        //
    }

    void IntegerLiteral::accept(Pass &visitor) {
        visitor.visitIntegerLiteral(this->dynamicCast<IntegerLiteral>());
    }
}
