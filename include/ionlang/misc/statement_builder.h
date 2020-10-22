#pragma once

#include <ionshared/misc/helpers.h>
#include <ionlang/construct/statement/return_statement.h>
#include <ionlang/construct/statement/assignment_statement.h>
#include <ionlang/construct/statement/if_statement.h>
#include <ionlang/construct/statement.h>
#include <ionlang/construct/block.h>
#include <ionlang/construct/type.h>

namespace ionlang {
    class StatementBuilder {
    private:
        std::shared_ptr<Block> block;

    public:
        explicit StatementBuilder(std::shared_ptr<Block> block);

        [[nodiscard]] std::shared_ptr<Block> getBlock() const noexcept;

        void appendStatement(const std::shared_ptr<Statement>& statement);

        template<class TStatement, typename... TArgs>
        std::shared_ptr<TStatement> make(TArgs... args) {
            // TODO: Ensure T inherits from Inst or derived.

            std::shared_ptr<TStatement> statement = std::make_shared<TStatement>(args...);

            this->appendStatement(statement);

            return statement;
        }

        std::shared_ptr<VariableDeclStatement> createVariableDecl(
            std::shared_ptr<Type> type,
            std::string name,
            std::shared_ptr<Expression<>> value
        );

        std::shared_ptr<AssignmentStatement> createAssignment(
            std::shared_ptr<VariableDeclStatement> variableDeclStatement,
            std::shared_ptr<Expression<>> value
        );

        std::shared_ptr<IfStatement> createIf(
            std::shared_ptr<Construct> condition,
            std::shared_ptr<Block> consequentBlock,
            ionshared::OptPtr<Block> alternativeBlock
        );

        std::shared_ptr<ReturnStatement> createReturn(
            ionshared::OptPtr<Expression<>> value = std::nullopt
        );
    };
}
