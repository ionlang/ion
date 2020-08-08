#include <ionlang/construct/type.h>

namespace ionlang {
    Type::Type(std::string id, TypeKind kind, bool isPointer)
        : Construct(ConstructKind::Type), ionshared::Named(id), kind(kind), isPointer(isPointer) {
        //
    }

    TypeKind Type::getTypeKind() const noexcept {
        return this->kind;
    }

    bool Type::getIsPointer() const noexcept {
        return this->isPointer;
    }

    void Type::setIsPointer(bool isPointer) noexcept {
        this->isPointer = isPointer;
    }
}
