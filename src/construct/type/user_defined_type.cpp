#include <ionlang/construct/type/boolean_type.h>
#include <ionlang/const/const_name.h>
#include <ionlang/passes/pass.h>

namespace ionlang {
    UserDefinedType::UserDefinedType(
        std::string name,
        ionshared::Ptr<TypeQualifiers> qualifiers
    ) noexcept :
        Type(name, TypeKind::UserDefined, std::move(qualifiers)) {
        //
    }

    void UserDefinedType::accept(Pass& pass) {
        return pass.visitUserDefinedType(this->dynamicCast<UserDefinedType>());
    }
}
