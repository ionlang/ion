#include <utility>
#include <ionlang/passes/pass.h>

namespace ionlang {
    Type::Type(std::string id, TypeKind kind)
        : Construct(ConstructKind::Type), ionshared::Named(std::move(id)), kind(kind) {
        //
    }

    void Type::accept(Pass &visitor) {
        visitor.visitType(this->dynamicCast<Type>());
    }

    TypeKind Type::getTypeKind() const noexcept {
        return this->kind;
    }
}
