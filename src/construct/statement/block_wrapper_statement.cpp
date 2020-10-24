#include <ionlang/passes/pass.h>

namespace ionlang {
    std::shared_ptr<BlockWrapperStmt> BlockWrapperStmt::make(
        const std::shared_ptr<Block>& block
    ) noexcept {
        std::shared_ptr<BlockWrapperStmt> result =
            std::make_shared<BlockWrapperStmt>(block);

        block->parent = result;

        return result;
    }

    BlockWrapperStmt::BlockWrapperStmt(std::shared_ptr<Block> block) :
        Statement(StatementKind::BlockWrapper),
        block(std::move(block)) {
        //
    }

    void BlockWrapperStmt::accept(Pass& visitor) {
        visitor.visitBlockWrapperStatement(this->dynamicCast<BlockWrapperStmt>());
    }

    Ast BlockWrapperStmt::getChildNodes() {
        return {this->block};
    }
}
