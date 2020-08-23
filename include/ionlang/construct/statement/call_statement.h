#pragma once

#include <ionlang/construct/statement.h>
#include <ionlang/construct/function.h>

namespace ionlang {
    class Pass;

    struct CallStatementOpts : StatementOpts {
        ionshared::Ptr<Function> callee;
    };

    class CallStatement : public Statement {
    private:
        ionshared::Ptr<Function> callee;

        // TODO: Missing call args?

    public:
        explicit CallStatement(const CallStatementOpts &opts);

        void accept(Pass &visitor) override;

        ionshared::Ptr<Function> getCallee() const noexcept;

        void setCallee(ionshared::Ptr<Function> callee) noexcept;
    };
}
