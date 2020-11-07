#include <utility>
#include <ionlang/passes/pass.h>

namespace ionlang {
    VariableRefExpr::VariableRefExpr(PtrResolvable<VariableDeclStmt> variableDecl) :
        Expression<>(
            ExpressionKind::VariableReference,

            // TODO: What if variableDecl Resolvable was resolved when being passed in? Then ->name would be std::nullopt.
            Resolvable<Type>::make(ResolvableKind::NearestVariableOrArgument, *variableDecl->id, nullptr)
        ),

        variableDecl(std::move(variableDecl)) {
        //
    }

    void VariableRefExpr::accept(Pass& visitor) {
        visitor.visitVariableRefExpr(this->dynamicCast<VariableRefExpr>());
    }

    Ast VariableRefExpr::getChildNodes() {
        return {this->variableDecl};
    }
}
