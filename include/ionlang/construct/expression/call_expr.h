#pragma once

#include "../statement.h"
#include "../function.h"

namespace ionlang {
    class Pass;

    struct CallExprOpts : StatementOpts {
        PtrRef<Function> callee;

        ionshared::Ptr<Value<>> args;
    };

    typedef std::vector<Value<>> CallArgs;

    class CallExpr : public Statement {
    private:
        PtrRef<Function> callee;

        ionshared::Ptr<CallArgs> args;

    public:
        explicit CallExpr(const CallExprOpts &opts);

        void accept(Pass &visitor) override;

        [[nodiscard]] PtrRef<Function> getCallee() const noexcept;

        void setCallee(PtrRef<Function> callee) noexcept;

        [[nodiscard]] ionshared::Ptr<CallArgs> getArgs() const noexcept;

        void setArgs(ionshared::Ptr<CallArgs> args) noexcept;
    };
}
