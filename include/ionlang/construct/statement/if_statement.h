#pragma once

#include <ionlang/construct/statement.h>

namespace ionlang {
    struct Pass;

    struct IfStmt : Statement {
        static std::shared_ptr<IfStmt> make(
            const std::shared_ptr<Construct>& condition,
            const std::shared_ptr<Block>& consequentBlock,
            const ionshared::OptPtr<Block>& alternativeBlock = std::nullopt
        ) noexcept;

        std::shared_ptr<Construct> condition;

        std::shared_ptr<Block> consequentBlock;

        ionshared::OptPtr<Block> alternativeBlock;

        IfStmt(
            std::shared_ptr<Construct> condition,
            std::shared_ptr<Block> consequentBlock,
            ionshared::OptPtr<Block> alternativeBlock = std::nullopt
        ) noexcept;

        void accept(Pass& visitor) override;

        [[nodiscard]] Ast getChildNodes() override;

        [[nodiscard]] bool hasAlternativeBlock() const noexcept;
    };
}
