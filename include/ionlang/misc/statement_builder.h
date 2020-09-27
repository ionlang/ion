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
        ionshared::Ptr<Block> block;

    public:
        explicit StatementBuilder(ionshared::Ptr<Block> block);

        [[nodiscard]] ionshared::Ptr<Block> getBlock() const noexcept;

        void appendStatement(const ionshared::Ptr<Statement> &statement);

        template<class TStatement, typename... TArgs>
        ionshared::Ptr<TStatement> make(TArgs... args) {
            // TODO: Ensure T inherits from Inst or derived.

            ionshared::Ptr<TStatement> statement = std::make_shared<TStatement>(args...);

            this->appendStatement(statement);

            return statement;
        }

        ionshared::Ptr<VariableDeclStatement> createVariableDecl(
            ionshared::Ptr<Type> type,
            std::string id,
            ionshared::Ptr<Construct> value
        );

        ionshared::Ptr<AssignmentStatement> createAssignment(
            ionshared::Ptr<VariableDeclStatement> variableDeclStatement,
            ionshared::Ptr<Construct> value
        );

        ionshared::Ptr<IfStatement> createIf(
            ionshared::Ptr<Construct> condition,
            ionshared::Ptr<Block> consequentBlock,
            ionshared::OptPtr<Block> alternativeBlock
        );

        ionshared::Ptr<ReturnStatement> createReturn(
            ionshared::OptPtr<Expression> value = std::nullopt
        );
    };
}
