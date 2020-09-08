#include <ionlang/passes/pass.h>
#include <ionlang/misc/util.h>

namespace ionlang {
    IntegerType::IntegerType(IntegerKind kind, bool isSigned, ionshared::Ptr<TypeQualifiers> qualifiers) :
        Type(util::resolveIntegerKindName(kind), TypeKind::Integer, std::move(qualifiers)),
        kind(kind),
        isSigned(isSigned) {
        //
    }

    void IntegerType::accept(Pass &pass) {
        return pass.visitIntegerType(this->dynamicCast<IntegerType>());
    }

    IntegerKind IntegerType::getIntegerKind() const noexcept {
        return this->kind;
    }

    bool IntegerType::getIsSigned() const noexcept {
        return this->isSigned;
    }

    void IntegerType::setIsSigned(bool isSigned) noexcept {
        this->isSigned = isSigned;
    }
}
