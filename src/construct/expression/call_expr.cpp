#include <utility>
#include <ionlang/passes/pass.h>

namespace ionlang {
    CallExpr::CallExpr(
        PtrResolvable<> calleeResolvable,
        CallArgs args,
        const PtrResolvable<Type>& type
    ) noexcept :
        Expression(ExpressionKind::Call, type),
        calleeResolvable(std::move(calleeResolvable)),
        args(std::move(args)) {
        //
    }

    void CallExpr::accept(Pass& visitor) {
        visitor.visitCallExpr(this->dynamicCast<CallExpr>());
    }

    Ast CallExpr::getChildNodes() {
        return {
            this->calleeResolvable
        };
    }
}
