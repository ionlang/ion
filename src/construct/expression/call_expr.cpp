#include <utility>
#include <ionlang/passes/pass.h>

namespace ionlang {
    CallExpr::CallExpr(PtrRef<Function> callee, CallArgs args) :
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

    PtrRef<Function> CallExpr::getCalleeRef() const noexcept {
        return this->calleeRef;
    }

    void CallExpr::setCalleeRef(PtrRef<Function> calleeRef) noexcept {
        this->calleeRef = std::move(calleeRef);
    }

    CallArgs CallExpr::getArgs() const noexcept {
        return this->args;
    }

    void CallExpr::setArgs(CallArgs args) noexcept {
        this->args = std::move(args);
    }
}
