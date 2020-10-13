#pragma once

#include <ionlang/construct/expression.h>
#include <ionlang/construct/function.h>

namespace ionlang {
    class Pass;

    struct VariableRefExpr : Expression<> {
        PtrResolvable<VariableDeclStatement> variableDecl;

        explicit VariableRefExpr(PtrResolvable<VariableDeclStatement> variableDecl);

        void accept(Pass& visitor) override;
    };
}
