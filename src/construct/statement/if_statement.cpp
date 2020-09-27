#include <ionlang/passes/pass.h>

namespace ionlang {
    IfStatement::IfStatement(const IfStatementOpts &opts) :
        Statement(opts.parent, StatementKind::If),
        condition(opts.condition),
        consequentBlock(opts.consequentBlock),
        alternativeBlock(opts.alternativeBlock) {
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

    bool IfStatement::hasAlternativeBlock() const noexcept {
        return ionshared::util::hasValue(this->alternativeBlock);
    }
}
