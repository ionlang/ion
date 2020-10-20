#pragma once

#include <ionlang/construct/expression.h>
#include <ionlang/construct/statement.h>

namespace ionlang {
    class Pass;

    struct ExprWrapperStatementOpts : StatementOpts {
        std::shared_ptr<Expression<>> expression;
    };

    struct ExprWrapperStatement : Statement {
        std::shared_ptr<Expression<>> expression;

        explicit ExprWrapperStatement(const ExprWrapperStatementOpts& opts);

        void accept(Pass& visitor) override;

        [[nodiscard]] Ast getChildNodes() override;
    };
}
