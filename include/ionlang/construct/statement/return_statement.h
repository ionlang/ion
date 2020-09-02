#pragma once

#include <ionlang/construct/statement.h>
#include <ionlang/construct/value.h>

namespace ionlang {
    class Pass;

    struct ReturnStatementOpts : StatementOpts {
        ionshared::OptPtr<Construct> value;
    };

    class ReturnStatement : public Statement {
    private:
        ionshared::OptPtr<Construct> value;

    public:
        explicit ReturnStatement(const ReturnStatementOpts &opts);

        void accept(Pass &visitor) override;

        Ast getChildNodes() override;

        ionshared::OptPtr<Construct> getValue() const noexcept;

        void setValue(ionshared::OptPtr<Construct> value) noexcept;

        bool hasValue() const noexcept;
    };
}
