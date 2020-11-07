#include <utility>
#include <ionlang/passes/pass.h>

namespace ionlang {
    std::shared_ptr<CallExpr> CallExpr::make(
        const PtrResolvable<>& calleeResolvable,
        const CallArgs& arguments,
        const PtrResolvable<Type>& type
    ) noexcept {
        std::shared_ptr<CallExpr> result =
            std::make_shared<CallExpr>(calleeResolvable, arguments, type);

        calleeResolvable->parent = result;

        for (const auto& argument : arguments) {
            argument->parent = result;
        }

        return result;
    }

    CallExpr::CallExpr(
        PtrResolvable<> calleeResolvable,
        CallArgs arguments,
        const PtrResolvable<Type>& type
    ) noexcept :
        Expression<>(ExpressionKind::Call, type),
        calleeResolvable(std::move(calleeResolvable)),
        arguments(std::move(arguments)) {
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
