#pragma once

#include <string>
#include <ionshared/misc/named.h>
#include <ionshared/misc/helpers.h>
#include <ionshared/tracking/symbol_table.h>
#include <ionshared/tracking/scoped.h>
#include <ionlang/construct/pseudo/child_construct.h>
#include "ionlang/construct/statement/variable_decl_statement.h"
#include "statement.h"

namespace ionlang {
    class Pass;

    class StatementBuilder;

    // TODO: Must be verified to contain a single terminal instruction at the end?
    class Block : public ChildConstruct<>, public ionshared::Scoped<VariableDeclStatement> {
    private:
        std::vector<ionshared::Ptr<Statement>> statements;

    public:
        explicit Block(
            ionshared::Ptr<Construct> parent,

            std::vector<ionshared::Ptr<Statement>> statements = {},

            const ionshared::PtrSymbolTable<VariableDeclStatement> &symbolTable =
                ionshared::util::makePtrSymbolTable<VariableDeclStatement>()
        );

        void accept(Pass &visitor) override;

        Ast getChildNodes() override;

        [[nodiscard]] std::vector<ionshared::Ptr<Statement>> &getStatements() noexcept;

        // TODO: When statements are set, the symbol table must be cleared and re-populated.
        void setStatements(std::vector<ionshared::Ptr<Statement>> statements);

        void appendStatement(const ionshared::Ptr<Statement> &statement);

        uint32_t relocateStatements(Block &target, uint32_t from = 0);

        /**
         * Splits the local basic block, relocating all instructions
         * at and after the provided index to a new basic block with
         * the provided id, having the same parent as the local basic
         * block.
         */
        [[nodiscard]] ionshared::Ptr<Block> split(uint32_t atOrder);

        /**
         * Attempt to find the index location of an instruction.
         * Returns null if not found.
         */
        [[nodiscard]] std::optional<uint32_t> locate(ionshared::Ptr<Statement> statement) const;

        [[nodiscard]] ionshared::Ptr<StatementBuilder> createBuilder();

        /**
         * Loops through all statements collecting terminal statements
         * (or expressions) such as function calls or return statements.
         * Search will be performed only on the local symbol table, so
         * nested blocks will be ignored.
         */
        [[nodiscard]] std::vector<ionshared::Ptr<Statement>> findTerminals() const;

        [[nodiscard]] ionshared::OptPtr<Statement> findFirstStatement() noexcept;

        [[nodiscard]] ionshared::OptPtr<Statement> findLastStatement() noexcept;

        [[nodiscard]] bool isFunctionBody() const;
    };
}
