#include <utility>
#include <ionlang/passes/pass.h>

namespace ionlang {
    Type::Type(std::string id, TypeKind kind, ionshared::Ptr<TypeQualifiers> qualifiers) :
        Construct(ConstructKind::Type),
        ionshared::Named(std::move(id)),
        kind(kind),
        qualifiers(std::move(qualifiers)) {
        //
    }

    void Type::accept(Pass &visitor) {
        visitor.visitType(this->dynamicCast<Type>());
    }

    TypeKind Type::getTypeKind() const noexcept {
        return this->kind;
    }

    ionshared::Ptr<TypeQualifiers> Type::getQualifiers() const noexcept {
        return this->qualifiers;
    }

    void Type::setQualifiers(ionshared::Ptr<TypeQualifiers> qualifiers) noexcept {
        this->qualifiers = std::move(qualifiers);
    }

    bool Type::addQualifier(TypeQualifier qualifier) noexcept {
        return this->qualifiers->insert(qualifier);
    }

    bool Type::removeQualifier(TypeQualifier qualifier) noexcept {
        return this->qualifiers->remove(qualifier);
    }

    bool Type::hasQualifier(TypeQualifier qualifier) const {
        return this->qualifiers->contains(qualifier);
    }
}
