#pragma once

#include <ionlang/construct/expression.h>
#include <ionlang/construct/function.h>

namespace ionlang {
    class Pass;

    typedef std::vector<ionshared::Ptr<Value<>>> CallArgs;

    class CallExpr : public Expression {
    private:
        PtrRef<Function> calleeRef;

        CallArgs args;

    public:
        CallExpr(PtrRef<Function> callee, CallArgs args);

        void accept(Pass &visitor) override;

        [[nodiscard]] Ast getChildNodes() override;

        [[nodiscard]] PtrRef<Function> getCalleeRef() const noexcept;

        void setCalleeRef(PtrRef<Function> calleeRef) noexcept;

        [[nodiscard]] CallArgs getArgs() const noexcept;

        void setArgs(CallArgs args) noexcept;
    };
}
