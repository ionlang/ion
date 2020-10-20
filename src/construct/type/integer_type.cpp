#include <ionlang/passes/pass.h>
#include <ionlang/misc/util.h>

namespace ionlang {
    IntegerType::IntegerType(IntegerKind kind, bool isSigned, std::shared_ptr<TypeQualifiers> qualifiers) :
        Type(util::resolveIntegerKindName(kind), TypeKind::Integer, std::move(qualifiers)),
        integerKind(kind),
        isSigned(isSigned) {
        //
    }

    void IntegerType::accept(Pass& pass) {
        return pass.visitIntegerType(this->dynamicCast<IntegerType>());
    }
}
