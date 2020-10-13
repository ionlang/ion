#pragma once

#include <ionlang/construct/pseudo/resolvable.h>
#include <ionlang/construct/expression.h>
#include <ionlang/construct/function.h>

namespace ionlang {
    class Pass;

    typedef std::vector<ionshared::Ptr<Expression<>>> CallArgs;

    struct CallExpr : Expression<> {
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
