#include <ionlang/passes/pass.h>

namespace ionlang {
    Statement::Statement(
        ionshared::Ptr<Block> parent,
        StatementKind kind,
        ionshared::OptPtr<Statement> yields
    ) :
        ChildConstruct(std::move(parent), ConstructKind::Statement),
        kind(kind),
        yields(std::move(yields)) {
        //
    }

    StatementKind Statement::getStatementKind() const noexcept {
        return this->kind;
    }

    ionshared::OptPtr<Statement> Statement::getYields() const noexcept {
        return this->yields;
    }

    bool Statement::isTerminal() const noexcept {
        return this->kind == StatementKind::Return;
    }

    uint32_t Statement::getOrder() {
        std::optional<uint32_t> order =
            this->getParent()->locate(this->dynamicCast<Statement>());

        if (!order.has_value()) {
            throw std::runtime_error("Could not locate instruction in parent");
        }

        return *order;
    }
}
