#pragma once

#include <ionlang/construct/expression.h>
#include <ionlang/construct/function.h>

namespace ionlang {
    struct Pass;

    struct VariableRefExpr : Expression<> {
        PtrResolvable<VariableDeclStmt> variableDecl;

        explicit VariableRefExpr(PtrResolvable<VariableDeclStmt> variableDecl);

        void accept(Pass& visitor) override;

        [[nodiscard]] Ast getChildNodes() override;
    };
}
