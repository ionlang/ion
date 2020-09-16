#pragma once

#include <ionlang/construct/expression.h>
#include <ionlang/construct/function.h>

namespace ionlang {
    class Pass;

    class VariableRefExpr : public Expression {
    private:
        PtrRef<VariableDeclStatement> variableDecl;

    public:
        explicit VariableRefExpr(PtrRef<VariableDeclStatement> variableDecl);

        void accept(Pass &visitor) override;

        [[nodiscard]] PtrRef<VariableDeclStatement> getVariableDecl() const noexcept;

        void setVariableDecl(PtrRef<VariableDeclStatement> variableDecl) noexcept;
    };
}
