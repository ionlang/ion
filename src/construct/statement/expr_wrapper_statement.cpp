#include <ionlang/passes/pass.h>

namespace ionlang {
    ExprWrapperStatement::ExprWrapperStatement(const ExprWrapperStatementOpts& opts) :
        Statement(opts.parent, StatementKind::ExprWrapper),
        expression(opts.expression) {
        //
    }

    void ExprWrapperStatement::accept(Pass& visitor) {
        visitor.visitExprWrapperStatement(this->dynamicCast<ExprWrapperStatement>());
    }

    Ast ExprWrapperStatement::getChildNodes() {
        return {
            this->expression
        };
    }
}
