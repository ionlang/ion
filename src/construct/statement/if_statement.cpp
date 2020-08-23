#include <ionlang/passes/pass.h>

namespace ionlang {
    IfStatement::IfStatement(const IfStatementOpts &opts)
        : Statement(opts.parent, StatementKind::If), condition(opts.condition) {
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

    ionshared::Ptr<Construct> IfStatement::getCondition() const noexcept {
        return this->condition;
    }

    void IfStatement::setCondition(ionshared::Ptr<Construct> condition) noexcept {
        this->condition = std::move(condition);
    }
}
