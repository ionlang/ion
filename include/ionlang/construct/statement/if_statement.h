#pragma once

#include <ionlang/construct/statement.h>

namespace ionlang {
    class Pass;

    struct IfStatementOpts : StatementOpts {
        std::shared_ptr<Construct> condition;

        std::shared_ptr<Block> consequentBlock;

        ionshared::OptPtr<Block> alternativeBlock;
    };

    struct IfStatement : Statement {
        std::shared_ptr<Construct> condition;

        std::shared_ptr<Block> consequentBlock;

        ionshared::OptPtr<Block> alternativeBlock;

        explicit IfStatement(const IfStatementOpts& opts);

        void accept(Pass& visitor) override;

        [[nodiscard]] Ast getChildNodes() override;

        [[nodiscard]] bool hasAlternativeBlock() const noexcept;
    };
}
