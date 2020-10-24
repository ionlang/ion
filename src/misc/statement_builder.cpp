#include <ionlang/misc/statement_builder.h>

namespace ionlang {
    StatementBuilder::StatementBuilder(std::shared_ptr<Block> block) :
        block(std::move(block)) {
        //
    }

    void StatementBuilder::appendStatement(const std::shared_ptr<Statement>& statement) {
        this->block->appendStatement(statement);
    }

    std::shared_ptr<VariableDeclStmt> StatementBuilder::createVariableDecl(
        std::shared_ptr<Type> type,
        std::string name,
        std::shared_ptr<Expression<>> value
    ) {
        return this->make<VariableDeclStmt>(
            std::move(Resolvable<Type>::make(std::move(type))),
            std::move(name),
            std::move(value)
        );
    }

    std::shared_ptr<AssignmentStmt> StatementBuilder::createAssignment(
        std::shared_ptr<VariableDeclStmt> variableDeclStatement,
        std::shared_ptr<Expression<>> value
    ) {
        return this->make<AssignmentStmt>(
            Resolvable<VariableDeclStmt>::make(std::move(variableDeclStatement)),
            std::move(value)
        );
    }

    std::shared_ptr<IfStmt> StatementBuilder::createIf(
        std::shared_ptr<Construct> condition,
        std::shared_ptr<Block> consequentBlock,
        ionshared::OptPtr<Block> alternativeBlock
    ) {
        return this->make<IfStmt>(
            std::move(condition),
            std::move(consequentBlock),
            std::move(alternativeBlock)
        );
    }

    std::shared_ptr<ReturnStmt> StatementBuilder::createReturn(
        ionshared::OptPtr<Expression<>> value
    ) {
        return this->make<ReturnStmt>(std::move(value));
    }
}
