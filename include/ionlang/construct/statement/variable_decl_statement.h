#pragma once

#include <string>
#include <ionshared/misc/helpers.h>
#include <ionlang/construct/expression.h>
#include "ionlang/construct/statement.h"

namespace ionlang {
    class Pass;

    struct VariableDeclStatementOpts : StatementOpts {
        ionshared::Ptr<Type> type;

        std::string id;

        ionshared::Ptr<Expression<>> value;
    };

    struct VariableDeclStatement : Statement, ionshared::Named {
        ionshared::Ptr<Type> type;

        ionshared::Ptr<Expression<>> value;

        explicit VariableDeclStatement(const VariableDeclStatementOpts& opts) noexcept;

        void accept(Pass& visitor) override;

        [[nodiscard]] Ast getChildNodes() override;
    };
}
