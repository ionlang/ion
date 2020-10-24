#pragma once

#include <ionlang/construct/pseudo/resolvable.h>
#include <ionlang/construct/expression.h>
#include <ionlang/construct/function.h>

namespace ionlang {
    struct Pass;

    typedef std::vector<std::shared_ptr<Expression<>>> CallArgs;

    struct CallExpr : Expression<> {
        static std::shared_ptr<CallExpr> make(
            const PtrResolvable<>& calleeResolvable,
            const CallArgs& args,
            const PtrResolvable<Type>& type
        ) noexcept;

        PtrResolvable<> calleeResolvable;

        CallArgs args;

        CallExpr(
            PtrResolvable<> calleeResolvable,
            CallArgs args,
            const PtrResolvable<Type>& type
        ) noexcept;

        void accept(Pass& visitor) override;

        [[nodiscard]] Ast getChildNodes() override;
    };
}
