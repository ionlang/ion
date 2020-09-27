#pragma once

#include <ionlang/construct/expression.h>
#include <ionlang/construct/function.h>

namespace ionlang {
    class Pass;

    typedef std::vector<ionshared::Ptr<Value<>>> CallArgs;

    struct CallExpr : Expression {
        PtrRef<> calleeRef;

        CallArgs args;

        CallExpr(PtrRef<> callee, CallArgs args);

        void accept(Pass &visitor) override;

        [[nodiscard]] Ast getChildNodes() override;
    };
}
