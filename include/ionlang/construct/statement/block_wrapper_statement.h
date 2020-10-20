#pragma once

#include <ionlang/construct/expression.h>
#include <ionlang/construct/statement.h>

namespace ionlang {
    class Pass;

    struct BlockWrapperStatementOpts : StatementOpts {
        std::shared_ptr<Block> block;
    };

    struct BlockWrapperStatement : Statement {
        std::shared_ptr<Block> block;

        explicit BlockWrapperStatement(const BlockWrapperStatementOpts &opts);

        void accept(Pass &visitor) override;

        [[nodiscard]] Ast getChildNodes() override;
    };
}
