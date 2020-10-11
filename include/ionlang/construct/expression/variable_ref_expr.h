#pragma once

#include <ionlang/construct/expression.h>
#include <ionlang/construct/function.h>

namespace ionlang {
    class Pass;

    class VariableRefExpr : public Expression {
    private:
        PtrResolvable<VariableDeclStatement> variableDecl;

    public:
        explicit VariableRefExpr(PtrResolvable<VariableDeclStatement> variableDecl);

        void accept(Pass &visitor) override;

        [[nodiscard]] PtrResolvable<VariableDeclStatement> getVariableDecl() const noexcept;

        void setVariableDecl(PtrResolvable<VariableDeclStatement> variableDecl) noexcept;
    };
}
