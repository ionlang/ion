#pragma once

#include <ionlang/construct/expression.h>
#include <ionlang/construct/statement.h>

namespace ionlang {
    struct Pass;

    struct ExprWrapperStmt : Statement {
        static std::shared_ptr<ExprWrapperStmt> make(
            const std::shared_ptr<Expression<>>& expression
        ) noexcept;

        std::shared_ptr<Expression<>> expression;

        explicit ExprWrapperStmt(std::shared_ptr<Expression<>> expression);

        void accept(Pass& visitor) override;

        [[nodiscard]] Ast getChildNodes() override;
    };
}
