#include <utility>
#include <ionlang/passes/pass.h>

namespace ionlang {
    CallExpr::CallExpr(PtrRef<> callee, CallArgs args) :
        // TODO: Expression requires 'type' but since the callee is a PtrRef, it's type is not necessarily resolved yet. What to do?
        Expression(ExpressionKind::Call, nullptr),

        calleeRef(std::move(callee)),
        args(std::move(args)) {
        //
    }

    void CallExpr::accept(Pass &visitor) {
        visitor.visitCallExpr(this->dynamicCast<CallExpr>());
    }

    Ast CallExpr::getChildNodes() {
        return {
            this->calleeRef
        };
    }
}
