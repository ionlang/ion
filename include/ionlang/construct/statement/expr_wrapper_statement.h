#pragma once

#include <ionlang/construct/expression.h>
#include <ionlang/construct/statement.h>

namespace ionlang {
    class Pass;

    struct ExprWrapperStatementOpts : StatementOpts {
        ionshared::Ptr<Expression> expression;
    };

    class ExprWrapperStatement : public Statement {
    private:
        ionshared::Ptr<Expression> expression;

    public:
        explicit ExprWrapperStatement(const ExprWrapperStatementOpts &opts);

        void accept(Pass &visitor) override;

        Ast getChildNodes() override;

        [[nodiscard]] ionshared::Ptr<Expression> getExpression() const noexcept;

        void setExpression(ionshared::Ptr<Expression> expression) noexcept;
    };
}
