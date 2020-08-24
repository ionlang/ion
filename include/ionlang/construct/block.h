#pragma once

#include <string>
#include <ionshared/misc/named.h>
#include <ionshared/misc/helpers.h>
#include <ionshared/tracking/symbol_table.h>
#include <ionshared/tracking/scope_anchor.h>
#include <ionlang/construct/pseudo/child_construct.h>
#include "ionlang/construct/statement/variable_declaration.h"
#include "statement.h"

namespace ionlang {
    class Pass;

    class StatementBuilder;

    // TODO: Must be verified to contain a single terminal instruction at the end?
    class Block : public ChildConstruct<>, public ionshared::ScopeAnchor<Statement> {
    private:
        std::vector<ionshared::Ptr<Statement>> statements;

    public:
        explicit Block(
            ionshared::Ptr<Construct> parent,

            std::vector<ionshared::Ptr<Statement>> statements = {},

            ionshared::PtrSymbolTable<Statement> symbolTable =
                ionshared::Util::makePtrSymbolTable<Statement>()
        );

        void accept(Pass &visitor) override;

        Ast getChildNodes() override;

        std::vector<ionshared::Ptr<Statement>> &getStatements() noexcept;

        // TODO: When statements are set, the symbol table must be cleared and re-populated.
        void setStatements(std::vector<ionshared::Ptr<Statement>> statements);

        void insertStatement(const ionshared::Ptr<Statement> &statement);

        uint32_t relocateStatements(Block &target, uint32_t from = 0);

        /**
         * Attempt to find the index location of an instruction.
         * Returns null if not found.
         */
        std::optional<uint32_t> locate(ionshared::Ptr<Statement> statement) const;

        ionshared::Ptr<StatementBuilder> createBuilder();

        ionshared::OptPtr<Statement> findTerminalStatement() const;

        bool isFunctionBody() const;
    };
}
