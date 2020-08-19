#include <ionlang/construct/type/boolean_type.h>
#include <ionlang/const/const_name.h>
#include <ionlang/passes/pass.h>

namespace ionlang {
    BooleanType::BooleanType(bool isPointer)
        : Type(ConstName::typeBool, TypeKind::Boolean, isPointer) {
        //
    }

    void BooleanType::accept(Pass &pass) {
        return pass.visitBooleanType(this->dynamicCast<BooleanType>());
    }
}
