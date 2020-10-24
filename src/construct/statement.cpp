#include <ionlang/passes/pass.h>

namespace ionlang {
    Statement::Statement(
        StatementKind kind,
        ionshared::OptPtr<Statement> yields
    ) :
        ConstructWithParent(ConstructKind::Statement),
        statementKind(kind),
        yields(std::move(yields)) {
        //
    }

    bool Statement::isTerminal() const noexcept {
        return this->statementKind == StatementKind::Return;
    }

    uint32_t Statement::getOrder() {
        std::optional<uint32_t> order =
            this->forceGetUnboxedParent()->locate(this->dynamicCast<Statement>());

        if (!order.has_value()) {
            throw std::runtime_error("Could not locate instruction in parent");
        }

        return *order;
    }
}
