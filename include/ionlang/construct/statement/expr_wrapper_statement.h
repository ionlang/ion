#pragma once

#include <ionlang/construct/expression.h>
#include <ionlang/construct/statement.h>

namespace ionlang {
    class Pass;

    struct ExprWrapperStatementOpts : StatementOpts {
        ionshared::Ptr<Expression<>> expression;
    };

    struct ExprWrapperStatement : Statement {
        ionshared::Ptr<Expression<>> expression;

        explicit ExprWrapperStatement(const ExprWrapperStatementOpts& opts);

        void accept(Pass& visitor) override;

        [[nodiscard]] Ast getChildNodes() override;
    };
}
