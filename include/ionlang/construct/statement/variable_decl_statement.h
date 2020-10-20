#pragma once

#include <string>
#include <ionshared/misc/helpers.h>
#include <ionlang/construct/expression.h>
#include "ionlang/construct/statement.h"

namespace ionlang {
    class Pass;

    struct VariableDeclStatementOpts : StatementOpts {
        std::shared_ptr<Type> type;

        std::string id;

        std::shared_ptr<Expression<>> value;
    };

    struct VariableDeclStatement : Statement, ionshared::Named {
        std::shared_ptr<Type> type;

        std::shared_ptr<Expression<>> value;

        explicit VariableDeclStatement(const VariableDeclStatementOpts& opts) noexcept;

        void accept(Pass& visitor) override;

        [[nodiscard]] Ast getChildNodes() override;
    };
}
