#include <ionlang/construct/type/boolean_type.h>
#include <ionlang/const/const_name.h>
#include <ionlang/passes/pass.h>

namespace ionlang {
    BooleanType::BooleanType(ionshared::Ptr<TypeQualifiers> qualifiers) :
        Type(ConstName::typeBool, TypeKind::Boolean, std::move(qualifiers)) {
        //
    }

    void BooleanType::accept(Pass &pass) {
        return pass.visitBooleanType(this->dynamicCast<BooleanType>());
    }
}
