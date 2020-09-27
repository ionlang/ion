#pragma once

#include <ionlang/construct/statement.h>

namespace ionlang {
    class Pass;

    struct IfStatementOpts : StatementOpts {
        ionshared::Ptr<Construct> condition;

        ionshared::Ptr<Block> consequentBlock;

        ionshared::OptPtr<Block> alternativeBlock;
    };

    struct IfStatement : Statement {
        ionshared::Ptr<Construct> condition;

        ionshared::Ptr<Block> consequentBlock;

        ionshared::OptPtr<Block> alternativeBlock;

        explicit IfStatement(const IfStatementOpts &opts);

        void accept(Pass &visitor) override;

        [[nodiscard]] Ast getChildNodes() override;

        [[nodiscard]] bool hasAlternativeBlock() const noexcept;
    };
}
