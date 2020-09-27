#include <utility>
#include <ionlang/passes/pass.h>

namespace ionlang {
    Type::Type(
        std::string id,
        TypeKind kind,
        ionshared::Ptr<TypeQualifiers> qualifiers
    ) :
        Construct(ConstructKind::Type),
        ionshared::Named{std::move(id)},
        typeKind(kind),
        qualifiers(std::move(qualifiers)) {
        //
    }

    void Type::accept(Pass &visitor) {
        visitor.visitType(this->dynamicCast<Type>());
    }

    TypeKind Type::getTypeKind() const noexcept {
        return this->typeKind;
    }

    bool Type::addQualifier(TypeQualifier qualifier) noexcept {
        return this->qualifiers->add(qualifier);
    }

    bool Type::removeQualifier(TypeQualifier qualifier) noexcept {
        return this->qualifiers->remove(qualifier);
    }

    bool Type::hasQualifier(TypeQualifier qualifier) const {
        return this->qualifiers->contains(qualifier);
    }
}
