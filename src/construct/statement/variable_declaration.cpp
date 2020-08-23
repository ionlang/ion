#include <utility>
#include <ionlang/passes/pass.h>

namespace ionlang {
    VariableDecl::VariableDecl(ionshared::Ptr<Block> parent, ionshared::Ptr<Type> type, std::string id, ionshared::Ptr<Value<>> value)
        : Statement(std::move(parent), StatementKind::VariableDeclaration), ionshared::Named(std::move(id)), type(std::move(type)), value(std::move(value)) {
        //
    }

    void VariableDecl::accept(Pass &visitor) {
        visitor.visitVariableDecl(this->dynamicCast<VariableDecl>());
    }

    ionshared::Ptr<Type> VariableDecl::getType() const noexcept {
        return this->type;
    }

    void VariableDecl::setType(ionshared::Ptr<Type> type) noexcept {
        this->type = std::move(type);
    }

    ionshared::Ptr<Value<>> VariableDecl::getValue() const noexcept {
        return this->value;
    }

    void VariableDecl::setValue(ionshared::Ptr<Value<>> value) noexcept {
        this->value = std::move(value);
    }
}
