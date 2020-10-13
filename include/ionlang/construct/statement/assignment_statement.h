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

        ionshared::Ptr<Expression<>> value;
    };

    struct AssignmentStatement : Statement {
        PtrResolvable<VariableDeclStatement> variableDeclStatementRef;

        ionshared::Ptr<Expression<>> value;

        explicit AssignmentStatement(const AssignmentStatementOpts &opts);

        void accept(Pass& visitor) override;

        [[nodiscard]] Ast getChildNodes() override;
    };
}
