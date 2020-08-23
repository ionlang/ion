#pragma once

#include <ionlang/construct/statement.h>

namespace ionlang {
    class Pass;

    struct IfStatementOpts : StatementOpts {
        ionshared::Ptr<Construct> condition;
    };

    class IfStatement : public Statement {
    private:
        ionshared::Ptr<Construct> condition;

    public:
        explicit IfStatement(const IfStatementOpts &opts);

        void accept(Pass &visitor) override;

        Ast getChildNodes() override;

        ionshared::Ptr<Construct> getCondition() const noexcept;

        void setCondition(ionshared::Ptr<Construct> value) noexcept;
    };
}
