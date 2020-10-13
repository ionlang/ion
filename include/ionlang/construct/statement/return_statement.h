#pragma once

#include <ionlang/construct/statement.h>
#include <ionlang/construct/expression.h>

namespace ionlang {
    class Pass;

    struct ReturnStatementOpts : StatementOpts {
        ionshared::OptPtr<Expression<>> value = std::nullopt;
    };

    struct ReturnStatement : Statement {
        ionshared::OptPtr<Expression<>> value;

        explicit ReturnStatement(const ReturnStatementOpts& opts);

        void accept(Pass& visitor) override;

        [[nodiscard]] Ast getChildNodes() override;

        [[nodiscard]] bool hasValue() const noexcept;
    };
}
