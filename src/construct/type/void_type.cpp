#include <ionlang/passes/pass.h>
#include <ionlang/const/const_name.h>

namespace ionlang {
    VoidType::VoidType()
        : Type(ConstName::typeVoid, TypeKind::Void, false) {
        //
    }

    void VoidType::accept(Pass &pass) {
        return pass.visitVoidType(this->dynamicCast<VoidType>());
    }
}
