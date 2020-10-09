#include <ionlang/passes/pass.h>
#include <ionlang/const/const_name.h>

namespace ionlang {
    VoidType::VoidType()
        : Type(const_name::typeVoid, TypeKind::Void) {
        //
    }

    void VoidType::accept(Pass &pass) {
        return pass.visitVoidType(this->dynamicCast<VoidType>());
    }
}
