#include <utility>
#include <ionlang/passes/pass.h>

namespace ionlang {
    AssignmentStatement::AssignmentStatement(
        ionshared::Ptr<Block> parent,
        PtrRef<VariableDecl> variableDecl,
        ionshared::Ptr<Construct> value
    ) :
        Statement(std::move(parent), StatementKind::Assignment),
        variableDecl(std::move(variableDecl)),
        value(std::move(value)) {
        //
    }

    void AssignmentStatement::accept(Pass &visitor) {
        visitor.visitVariableDecl(this->dynamicCast<VariableDecl>());
    }

    PtrRef<VariableDecl> AssignmentStatement::getVariableDecl() const noexcept {
        return this->variableDecl;
    }

    void AssignmentStatement::setVariableDecl(PtrRef<VariableDecl> variableDecl) noexcept {
        this->variableDecl = std::move(variableDecl);
    }

    ionshared::Ptr<Construct> AssignmentStatement::getValue() const noexcept {
        return this->value;
    }

    void AssignmentStatement::setValue(ionshared::Ptr<Construct> value) noexcept {
        this->value = std::move(value);
    }
}
