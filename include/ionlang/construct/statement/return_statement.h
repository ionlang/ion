#pragma once

#include <ionlang/construct/statement.h>
#include <ionlang/construct/value.h>

namespace ionlang {
    class Pass;

    struct ReturnStatementOpts : StatementOpts {
        ionshared::OptPtr<Value<>> value;
    };

    class ReturnStatement : public Statement {
    private:
        ionshared::OptPtr<Value<>> value;

    public:
        explicit ReturnStatement(const ReturnStatementOpts &opts);

        void accept(Pass &visitor) override;

        Ast getChildNodes() override;

        ionshared::OptPtr<Value<>> getValue() const noexcept;

        void setValue(ionshared::OptPtr<Value<>> value) noexcept;

        bool hasValue() const noexcept;
    };
}
