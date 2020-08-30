#include <ionlang/passes/pass.h>

namespace ionlang {
    IfStatement::IfStatement(const IfStatementOpts &opts)
        : Statement(opts.parent, StatementKind::If), condition(opts.condition), consequentBlock(opts.consequentBlock), alternativeBlock(opts.alternativeBlock) {
        //
    }

    void IfStatement::accept(Pass &visitor) {
        visitor.visitIfStatement(this->dynamicCast<IfStatement>());
    }

    Ast IfStatement::getChildNodes() {
        return {
            this->condition
        };
    }

    ionshared::Ptr<Value<>> IfStatement::getCondition() const noexcept {
        return this->condition;
    }

    void IfStatement::setCondition(ionshared::Ptr<Value<>> condition) noexcept {
        this->condition = std::move(condition);
    }

    ionshared::Ptr<Block> IfStatement::getConsequentBlock() const noexcept {
        return this->consequentBlock;
    }

    void IfStatement::setConsequentBlock(ionshared::Ptr<Block> block) noexcept {
        this->consequentBlock = std::move(block);
    }

    ionshared::OptPtr<Block> IfStatement::getAlternativeBlock() const noexcept {
        return this->alternativeBlock;
    }

    void IfStatement::setAlternativeBlock(ionshared::OptPtr<Block> block) noexcept {
        this->alternativeBlock = std::move(block);
    }

    bool IfStatement::hasAlternativeBlock() const noexcept {
        return ionshared::util::hasValue(this->getAlternativeBlock());
    }
}
