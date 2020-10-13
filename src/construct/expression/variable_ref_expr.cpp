#include <utility>
#include <ionlang/passes/pass.h>

namespace ionlang {
    VariableRefExpr::VariableRefExpr(PtrResolvable<VariableDeclStatement> variableDecl) :
        Expression<>(
            ExpressionKind::VariableReference,

            // TODO: What if variableDecl Resolvable was resolved when being passed in? Then ->name would be std::nullopt.
            Resolvable<Type>::make(ResolvableKind::Variable, *variableDecl->name, nullptr)
        ),

        variableDecl(std::move(variableDecl)) {
        //
    }

    void VariableRefExpr::accept(Pass& visitor) {
        visitor.visitVariableRefExpr(this->dynamicCast<VariableRefExpr>());
    }
}
