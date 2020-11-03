#pragma once

#include <ionshared/misc/helpers.h>
#include <ionlang/construct/statement/return_statement.h>
#include <ionlang/construct/statement/assignment_statement.h>
#include <ionlang/construct/statement/if_statement.h>
#include <ionlang/construct/statement.h>
#include <ionlang/construct/block.h>
#include <ionlang/construct/type.h>

namespace ionlang {
    struct StatementBuilder {
        std::shared_ptr<Block> block;

        explicit StatementBuilder(std::shared_ptr<Block> block);

        void appendStatement(const std::shared_ptr<Statement>& statement);

        template<class TStatement, typename... TArgs>
        std::shared_ptr<TStatement> make(TArgs... args) {
            // TODO: Ensure T inherits from Inst or derived.

            // TODO: Should be invoking TStatement::make() for certain statements.
            std::shared_ptr<TStatement> statement =
                Construct::makeChild<TStatement>(this->block, args...);

            this->appendStatement(statement);

            return statement;
        }

        std::shared_ptr<VariableDeclStmt> createVariableDecl(
            std::shared_ptr<Type> type,
            std::string name,
            std::shared_ptr<Expression<>> value
        );

        std::shared_ptr<AssignmentStmt> createAssignment(
            std::shared_ptr<VariableDeclStmt> variableDeclStatement,
            std::shared_ptr<Expression<>> value
        );

        std::shared_ptr<IfStmt> createIf(
            std::shared_ptr<Construct> condition,
            std::shared_ptr<Block> consequentBlock,
            ionshared::OptPtr<Block> alternativeBlock
        );

        std::shared_ptr<ReturnStmt> createReturn(
            ionshared::OptPtr<Expression<>> value = std::nullopt
        );
    };
}
