#include <ionlang/passes/pass.h>

namespace ionlang {
    CallStatement::CallStatement(const CallStatementOpts &opts)
        : Statement(opts.parent, StatementKind::Call), callee(opts.callee) {
        //
    }

    void CallStatement::accept(Pass &visitor) {
        visitor.visitCallStatement(this->dynamicCast<CallStatement>());
    }

    ionshared::Ptr<Function> CallStatement::getCallee() const noexcept {
        return this->callee;
    }

    void CallStatement::setCallee(ionshared::Ptr<Function> callee) noexcept {
        this->callee = std::move(callee);
    }
}
