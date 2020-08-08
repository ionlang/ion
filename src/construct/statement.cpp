#include <ionlang/passes/pass.h>

namespace ionlang {
    Statement::Statement(ionshared::Ptr<BasicBlock> parent, StatementKind kind, ionshared::OptPtr<Statement> yields)
        : ChildConstruct(parent, ConstructKind::Statement), kind(kind), yields(yields) {
        //
    }

    StatementKind Statement::getStatementKind() const noexcept {
        return this->kind;
    }

    ionshared::OptPtr<Statement> Statement::getYields() const noexcept {
        return std::nullopt;
    }

    bool Statement::isTerminal() const noexcept {
        return this->kind == StatementKind::Return;
    }
}
