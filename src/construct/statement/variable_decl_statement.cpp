#include <utility>
#include <ionlang/passes/pass.h>

namespace ionlang {
    VariableDeclStatement::VariableDeclStatement(const VariableDeclStatementOpts &opts) :
        Statement(opts.parent, StatementKind::VariableDeclaration),
        ionshared::Named(opts.id),
        type(opts.type),
        value(opts.value) {
        //
    }

    void VariableDeclStatement::accept(Pass &visitor) {
        visitor.visitVariableDecl(this->dynamicCast<VariableDeclStatement>());
    }

    ionshared::Ptr<Type> VariableDeclStatement::getType() const noexcept {
        return this->type;
    }

    void VariableDeclStatement::setType(ionshared::Ptr<Type> type) noexcept {
        this->type = std::move(type);
    }

    ionshared::Ptr<Construct> VariableDeclStatement::getValue() const noexcept {
        return this->value;
    }

    void VariableDeclStatement::setValue(ionshared::Ptr<Construct> value) noexcept {
        this->value = std::move(value);
    }
}
