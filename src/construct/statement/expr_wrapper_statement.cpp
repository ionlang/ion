#include <ionlang/passes/pass.h>

namespace ionlang {
    ExprWrapperStatement::ExprWrapperStatement(const ExprWrapperStatementOpts &opts) :
        Statement(opts.parent, StatementKind::ExprWrapper),
        expression(opts.expression) {
        //
    }

    void ExprWrapperStatement::accept(Pass &visitor) {
        visitor.visitExprWrapperStatement(this->dynamicCast<ExprWrapperStatement>());
    }

    Ast ExprWrapperStatement::getChildNodes() {
        return {
            this->expression
        };
    }

    ionshared::Ptr<Expression> ExprWrapperStatement::getExpression() const noexcept {
        return this->expression;
    }

    void ExprWrapperStatement::setExpression(ionshared::Ptr<Expression> expression) noexcept {
        this->expression = std::move(expression);
    }
}
