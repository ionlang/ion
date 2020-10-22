#pragma once

#include <string>
#include <ionshared/misc/helpers.h>
#include <ionlang/construct/expression.h>
#include "ionlang/construct/statement.h"

namespace ionlang {
    struct Pass;

    struct VariableDeclStatementOpts : StatementOpts {
        PtrResolvable<Type> type;

        std::string id;

        std::shared_ptr<Expression<>> value;
    };

    struct VariableDeclStatement : Statement, ionshared::Named {
        PtrResolvable<Type> type;

        std::shared_ptr<Expression<>> value;

        explicit VariableDeclStatement(const VariableDeclStatementOpts& opts) noexcept;

        void accept(Pass& visitor) override;

        [[nodiscard]] Ast getChildNodes() override;
    };
}
