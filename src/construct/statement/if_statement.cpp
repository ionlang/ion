#include <ionlang/passes/pass.h>

namespace ionlang {
    std::shared_ptr<IfStmt> IfStmt::make(
        const std::shared_ptr<Construct>& condition,
        const std::shared_ptr<Block>& consequentBlock,
        const ionshared::OptPtr<Block>& alternativeBlock
    ) noexcept {
        std::shared_ptr<IfStmt> result =
            std::make_shared<IfStmt>(condition, consequentBlock, alternativeBlock);

        condition->parent = result;
        consequentBlock->parent = result;

        if (ionshared::util::hasValue(alternativeBlock)) {
            alternativeBlock->get()->parent = result;
        }

        return result;
    }

    IfStmt::IfStmt(
        std::shared_ptr<Construct> condition,
        std::shared_ptr<Block> consequentBlock,
        ionshared::OptPtr<Block> alternativeBlock
    ) noexcept :
        Statement(StatementKind::If),
        condition(std::move(condition)),
        consequentBlock(std::move(consequentBlock)),
        alternativeBlock(std::move(alternativeBlock)) {
        //
    }

    void IfStmt::accept(Pass& visitor) {
        visitor.visitIfStmt(this->dynamicCast<IfStmt>());
    }

    Ast IfStmt::getChildNodes() {
        // TODO: Children should also include the blocks (which ARE created by the if statement?)?
        return {this->condition};
    }

    bool IfStmt::hasAlternativeBlock() const noexcept {
        return ionshared::util::hasValue(this->alternativeBlock);
    }
}
