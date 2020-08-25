#include <ionlang/passes/pass.h>

namespace ionlang {
    CallExpr::CallExpr(const CallExprOpts &opts)
        : Statement(opts.parent, StatementKind::Call), callee(opts.callee) {
        //
    }

    void CallExpr::accept(Pass &visitor) {
        visitor.visitCallExpr(this->dynamicCast<CallExpr>());
    }

    PtrRef<Function> CallExpr::getCallee() const noexcept {
        return this->callee;
    }

    void CallExpr::setCallee(PtrRef<Function> callee) noexcept {
        this->callee = std::move(callee);
    }

    ionshared::Ptr<CallArgs> CallExpr::getArgs() const noexcept {
        return this->args;
    }

    void CallExpr::setArgs(ionshared::Ptr<CallArgs> args) noexcept {
        this->args = std::move(args);
    }
}
