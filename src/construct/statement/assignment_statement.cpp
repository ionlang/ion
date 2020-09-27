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
        visitor.visitAssignmentStatement(this->dynamicCast<AssignmentStatement>());
    }

    Ast AssignmentStatement::getChildNodes() {
        return {
            this->variableDeclStatementRef
        };
    }
}
