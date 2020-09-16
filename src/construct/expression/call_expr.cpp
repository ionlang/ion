#include <utility>
#include <ionlang/passes/pass.h>

namespace ionlang {
    CallExpr::CallExpr(PtrRef<Function> callee, CallArgs args) :
        // TODO: Expression requires 'type' but since the callee is a PtrRef, it's type is not necessarily resolved yet. What to do?
        Expression(ExpressionKind::Call, nullptr),

        callee(std::move(callee)),
        args(std::move(args)) {
        //
    }

    void CallExpr::accept(Pass &visitor) {
        visitor.visitCallExpr(this->dynamicCast<CallExpr>());
    }

    PtrRef<Function> CallExpr::getCalleeRef() const noexcept {
        return this->callee;
    }

    void CallExpr::setCallee(PtrRef<Function> callee) noexcept {
        this->callee = std::move(callee);
    }

    CallArgs CallExpr::getArgs() const noexcept {
        return this->args;
    }

    void CallExpr::setArgs(CallArgs args) noexcept {
        this->args = std::move(args);
    }
}
