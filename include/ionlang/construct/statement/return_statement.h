#pragma once

#include <ionlang/construct/statement.h>
#include <ionlang/construct/expression.h>

namespace ionlang {
    struct Pass;

    struct ReturnStmt : Statement {
        static std::shared_ptr<ReturnStmt> make(
            ionshared::OptPtr<Expression<>> value
        ) noexcept;

        ionshared::OptPtr<Expression<>> value;

        explicit ReturnStmt(
            ionshared::OptPtr<Expression<>> value = std::nullopt
        ) noexcept;

        void accept(Pass& visitor) override;

        [[nodiscard]] Ast getChildNodes() override;

        [[nodiscard]] bool hasValue() const noexcept;
    };
}
