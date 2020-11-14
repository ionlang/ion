#include <ionlang/passes/pass.h>

namespace ionlang {
    std::shared_ptr<ExprWrapperStmt> ExprWrapperStmt::make(
        const std::shared_ptr<Expression<>>& expression
    ) noexcept {
        std::shared_ptr<ExprWrapperStmt> result =
            std::make_shared<ExprWrapperStmt>(expression);

        expression->setParent(result);

        return result;
    }

    ExprWrapperStmt::ExprWrapperStmt(std::shared_ptr<Expression<>> expression) :
        Statement(StatementKind::ExprWrapper),
        expression(std::move(expression)) {
        //
    }

    void ExprWrapperStmt::accept(Pass& visitor) {
        visitor.visitExprWrapperStmt(this->dynamicCast<ExprWrapperStmt>());
    }

    Ast ExprWrapperStmt::getChildNodes() {
        return {this->expression};
    }
}
