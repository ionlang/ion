#include <utility>
#include <ionlang/passes/pass.h>

namespace ionlang {
    AssignmentStatement::AssignmentStatement(const AssignmentStatementOpts &opts) :
        Statement(opts.parent, StatementKind::Assignment),
        variableDeclStatementRef(opts.variableDeclStatementRef),
        value(opts.value) {
        //
    }

    void AssignmentStatement::accept(Pass &visitor) {
        visitor.visitVariableDecl(this->dynamicCast<VariableDeclStatement>());
    }

    Ast AssignmentStatement::getChildNodes() {
        return {
            this->variableDeclStatementRef
        };
    }

    PtrRef<VariableDeclStatement> AssignmentStatement::getVariableDeclStatement() const noexcept {
        return this->variableDeclStatementRef;
    }

    void AssignmentStatement::setVariableDeclStatement(PtrRef<VariableDeclStatement> variableDeclStatement) noexcept {
        this->variableDeclStatementRef = std::move(variableDeclStatement);
    }

    ionshared::Ptr<Construct> AssignmentStatement::getValue() const noexcept {
        return this->value;
    }

    void AssignmentStatement::setValue(ionshared::Ptr<Construct> value) noexcept {
        this->value = std::move(value);
    }
}
