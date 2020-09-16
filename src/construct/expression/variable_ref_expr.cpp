#include <utility>
#include <ionlang/passes/pass.h>

namespace ionlang {
    VariableRefExpr::VariableRefExpr(PtrRef<VariableDeclStatement> variableDecl) :
        // TODO: Expression requires 'type' but since the callee is a PtrRef, it's type is not necessarily resolved yet. What to do?
        Expression(ExpressionKind::Call, nullptr),
        variableDecl(std::move(variableDecl)) {
        //
    }

    void VariableRefExpr::accept(Pass &visitor) {
        visitor.visitVariableRefExpr(this->dynamicCast<VariableRefExpr>());
    }

    PtrRef<VariableDeclStatement> VariableRefExpr::getVariableDecl() const noexcept {
        return this->variableDecl;
    }

    void VariableRefExpr::setVariableDecl(PtrRef<VariableDeclStatement> variableDecl) noexcept {
        this->variableDecl = std::move(variableDecl);
    }
}
