#include <ionlang/misc/statement_builder.h>

namespace ionlang {
    StatementBuilder::StatementBuilder(std::shared_ptr<Block> block) :
        block(std::move(block)) {
        //
    }

    std::shared_ptr<Block> StatementBuilder::getBlock() const noexcept {
        return this->block;
    }

    void StatementBuilder::appendStatement(const std::shared_ptr<Statement> &statement) {
        this->block->appendStatement(statement);
    }

    std::shared_ptr<VariableDeclStatement> StatementBuilder::createVariableDecl(
        std::shared_ptr<Type> type,
        std::string name,
        std::shared_ptr<Expression<>> value
    ) {
        return this->make<VariableDeclStatement, VariableDeclStatementOpts>(VariableDeclStatementOpts{
            this->block,
            std::move(Resolvable<Type>::make(std::move(type))),
            std::move(name),
            std::move(value)
        });
    }

    std::shared_ptr<AssignmentStatement> StatementBuilder::createAssignment(
        std::shared_ptr<VariableDeclStatement> variableDeclStatement,
        std::shared_ptr<Expression<>> value
    ) {
        return this->make<AssignmentStatement, AssignmentStatementOpts>(AssignmentStatementOpts{
            this->block,
            Resolvable<VariableDeclStatement>::make(std::move(variableDeclStatement)),
            std::move(value)
        });
    }

    std::shared_ptr<IfStatement> StatementBuilder::createIf(
        std::shared_ptr<Construct> condition,
        std::shared_ptr<Block> consequentBlock,
        ionshared::OptPtr<Block> alternativeBlock
    ) {
        return this->make<IfStatement, IfStatementOpts>(IfStatementOpts{
            this->block,
            std::move(condition),
            std::move(consequentBlock),
            std::move(alternativeBlock)
        });
    }

    std::shared_ptr<ReturnStatement> StatementBuilder::createReturn(ionshared::OptPtr<Expression<>> value) {
        return this->make<ReturnStatement, ReturnStatementOpts>(ReturnStatementOpts{
            this->block,
            std::move(value)
        });
    }
}
