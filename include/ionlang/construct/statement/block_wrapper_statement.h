#pragma once

#include <ionlang/construct/expression.h>
#include <ionlang/construct/statement.h>

namespace ionlang {
    struct Pass;

    struct BlockWrapperStmt : Statement {
        static std::shared_ptr<BlockWrapperStmt> make(
            const std::shared_ptr<Block>& block
        ) noexcept;

        std::shared_ptr<Block> block;

        explicit BlockWrapperStmt(std::shared_ptr<Block> block);

        void accept(Pass& visitor) override;

        [[nodiscard]] Ast getChildNodes() override;
    };
}
