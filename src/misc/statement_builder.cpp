#include <ionlang/misc/statement_builder.h>

namespace ionlang {
    StatementBuilder::StatementBuilder(ionshared::Ptr<Block> block) :
        block(std::move(block)) {
        //
    }

    ionshared::Ptr<Block> StatementBuilder::getBlock() const noexcept {
        return this->block;
    }

    void StatementBuilder::appendStatement(const ionshared::Ptr<Statement> &statement) {
        this->block->appendStatement(statement);
    }

    ionshared::Ptr<VariableDeclStatement> StatementBuilder::createVariableDecl(
        ionshared::Ptr<Type> type,
        std::string id,
        ionshared::Ptr<Expression<>> value
    ) {
        return this->make<VariableDeclStatement, VariableDeclStatementOpts>(VariableDeclStatementOpts{
            this->block,
            std::move(type),
            std::move(id),
            std::move(value)
        });
    }

    ionshared::Ptr<AssignmentStatement> StatementBuilder::createAssignment(
        ionshared::Ptr<VariableDeclStatement> variableDeclStatement,
        ionshared::Ptr<Expression<>> value
    ) {
        return this->make<AssignmentStatement, AssignmentStatementOpts>(AssignmentStatementOpts{
            this->block,
            Resolvable<VariableDeclStatement>::make(std::move(variableDeclStatement)),
            std::move(value)
        });
    }

    ionshared::Ptr<IfStatement> StatementBuilder::createIf(
        ionshared::Ptr<Construct> condition,
        ionshared::Ptr<Block> consequentBlock,
        ionshared::OptPtr<Block> alternativeBlock
    ) {
        return this->make<IfStatement, IfStatementOpts>(IfStatementOpts{
            this->block,
            std::move(condition),
            std::move(consequentBlock),
            std::move(alternativeBlock)
        });
    }

    ionshared::Ptr<ReturnStatement> StatementBuilder::createReturn(ionshared::OptPtr<Expression<>> value) {
        return this->make<ReturnStatement, ReturnStatementOpts>(ReturnStatementOpts{
            this->block,
            std::move(value)
        });
    }
}
