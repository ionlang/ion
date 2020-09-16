#pragma once

#include <ionlang/construct/expression.h>
#include <ionlang/construct/function.h>

namespace ionlang {
    class Pass;

    typedef std::vector<ionshared::Ptr<Value<>>> CallArgs;

    class CallExpr : public Expression {
    private:
        PtrRef<Function> callee;

        CallArgs args;

    public:
        explicit CallExpr(PtrRef<Function> callee, CallArgs args);

        void accept(Pass &visitor) override;



        [[nodiscard]] PtrRef<Function> getCalleeRef() const noexcept;

        void setCallee(PtrRef<Function> callee) noexcept;

        [[nodiscard]] CallArgs getArgs() const noexcept;

        void setArgs(CallArgs args) noexcept;
    };
}
