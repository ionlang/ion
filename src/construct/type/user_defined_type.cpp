#include <ionlang/construct/type/boolean_type.h>
#include <ionlang/const/const_name.h>
#include <ionlang/passes/pass.h>

namespace ionlang {
    UserDefinedType::UserDefinedType(ionshared::Ptr<TypeQualifiers> qualifiers) :
        // TODO: Proper construction of parent.
        Type(const_name::typeBool, TypeKind::Boolean, std::move(qualifiers)) {
        //
    }

    void UserDefinedType::accept(Pass& pass) {
        return pass.visitUserDefinedType(this->dynamicCast<UserDefinedType>());
    }
}
