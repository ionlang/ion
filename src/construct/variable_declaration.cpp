#include <ionlang/passes/pass.h>

namespace ionlang {
    VariableDeclaration::VariableDeclaration(std::string id, ionshared::Ptr<Construct> value)
        : Construct(ConstructKind::VariableDecl), id(std::move(id)), value(std::move(value)) {
        //
    }

    void VariableDeclaration::accept(Pass &visitor) {
        visitor.visitVariableDecl(this->dynamicCast<VariableDeclaration>());
    }

    std::string VariableDeclaration::getId() const noexcept {
        return this->id;
    }

    void VariableDeclaration::setId(std::string value) noexcept {
        this->id = std::move(value);
    }

    ionshared::Ptr<Construct> VariableDeclaration::getValue() const noexcept {
        return this->value;
    }

    void VariableDeclaration::setValue(ionshared::Ptr<Construct> value) noexcept {
        this->value = std::move(value);
    }
}
