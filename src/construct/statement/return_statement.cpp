#include <ionlang/passes/pass.h>

namespace ionlang {
    ReturnStatement::ReturnStatement(const ReturnStatementOpts &opts) :
        Statement(opts.parent, StatementKind::Return),
        value(opts.value) {
        //
    }

    void ReturnStatement::accept(Pass &visitor) {
        visitor.visitReturnStatement(this->dynamicCast<ReturnStatement>());
    }

    Ast ReturnStatement::getChildNodes() {
        if (!this->hasValue()) {
            return Construct::getChildNodes();
        }

        return {
            *this->value
        };
    }

    ionshared::OptPtr<Construct> ReturnStatement::getValue() const noexcept {
        return this->value;
    }

    void ReturnStatement::setValue(ionshared::OptPtr<Construct> value) noexcept {
        this->value = std::move(value);
    }

    bool ReturnStatement::hasValue() const noexcept {
        return ionshared::util::hasValue(this->value);
    }
}
