#include <ionlang/passes/pass.h>

namespace ionlang {
    std::shared_ptr<AssignmentStmt> AssignmentStmt::make(
        const PtrResolvable<VariableDeclStmt>& variableDeclStatementRef,
        const std::shared_ptr<Expression<>>& value
    ) noexcept {
        std::shared_ptr<AssignmentStmt> result =
            std::make_shared<AssignmentStmt>(variableDeclStatementRef, value);

        value->setParent(result);

        return result;
    }

    AssignmentStmt::AssignmentStmt(
        PtrResolvable<VariableDeclStmt> variableDeclStmt,
        std::shared_ptr<Expression<>> value
    ) :
        Statement(StatementKind::Assignment),
        variableDeclStmtRef(std::move(variableDeclStmt)),
        value(std::move(value)) {
        //
    }

    void AssignmentStmt::accept(Pass& visitor) {
        visitor.visitAssignmentStmt(this->dynamicCast<AssignmentStmt>());
    }

    Ast AssignmentStmt::getChildNodes() {
        return {
            this->variableDeclStmtRef
        };
    }
}
