#pragma once

#include <string>
#include <ionshared/misc/helpers.h>
#include <ionlang/construct/pseudo/resolvable.h>
#include <ionlang/construct/statement.h>
#include "variable_decl_statement.h"

namespace ionlang {
    class Pass;

    struct AssignmentStatementOpts : StatementOpts {
        PtrResolvable<VariableDeclStatement> variableDeclStatementRef;

        std::shared_ptr<Expression<>> value;
    };

    struct AssignmentStatement : Statement {
        PtrResolvable<VariableDeclStatement> variableDeclStatementRef;

        std::shared_ptr<Expression<>> value;

        explicit AssignmentStatement(const AssignmentStatementOpts &opts);

        void accept(Pass& visitor) override;

        [[nodiscard]] Ast getChildNodes() override;
    };
}
