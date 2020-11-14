#pragma once

#include <string>
#include <ionshared/misc/named.h>
#include <ionshared/misc/helpers.h>
#include <ionshared/tracking/symbol_table.h>
#include <ionshared/tracking/scoped.h>
#include <ionlang/construct/construct.h>
#include "module.h"

namespace ionlang {
    struct Pass;

    struct Statement;

    struct StatementBuilder;

    struct Function;

    // TODO: Must be verified to contain a single terminal instruction at the end?
    struct Block : ScopedConstruct {
        static std::shared_ptr<Block> make(
            const std::vector<std::shared_ptr<Statement>>& statements = {},

            const ionshared::PtrSymbolTable<Construct>& symbolTable =
            ionshared::util::makePtrSymbolTable<Construct>()
        ) noexcept;

        // TODO: When statements are mutated, the symbol table must be cleared and re-populated.
        std::vector<std::shared_ptr<Statement>> statements;

        explicit Block(
            std::vector<std::shared_ptr<Statement>> statements = {},

            const ionshared::PtrSymbolTable<Construct>& symbolTable =
                ionshared::util::makePtrSymbolTable<Construct>()
        );

        void accept(Pass& visitor) override;

        [[nodiscard]] Ast getChildNodes() override;

        /**
         * Append a statement to the local statement vector, and if
         * applicable, register it on the local symbol table. No
         * relocation is performed, and the statement's parent
         * is left intact.
         */
        void appendStatement(const std::shared_ptr<Statement>& statement);

        /**
         * Move the statement at the provided order index from this block
         * to another. The statement will be removed from the local vector,
         * and registered on the target block's symbol table.
         */
        bool relocateStatement(size_t orderIndex, const std::shared_ptr<Block>& target);

        size_t relocateStatements(
            const std::shared_ptr<Block>& target,
            size_t from = 0,
            std::optional<size_t> to = std::nullopt
        );

        /**
         * Splits the local basic block, relocating all instructions
         * within the provided range (or all after the starting index if
         * no end index was provided) to a new basic block with the same
         * parent as this local block.
         */
        [[nodiscard]] std::shared_ptr<Block> slice(
            size_t from,
            std::optional<size_t> to = std::nullopt
        );

        /**
         * Attempt to find the index location of a statement. Returns null
         * if not found.
         */
        [[nodiscard]] std::optional<size_t> locate(std::shared_ptr<Statement> statement) const;

        [[nodiscard]] std::shared_ptr<StatementBuilder> createBuilder();

        /**
         * Loops through all statements collecting terminal statements
         * (or expressions) such as function calls or return statements.
         * Search will be performed only on the local symbol table, so
         * nested blocks will be ignored.
         */
        [[nodiscard]] std::vector<std::shared_ptr<Statement>> findTerminals() const;

        [[nodiscard]] ionshared::OptPtr<Statement> findFirstStatement() noexcept;

        [[nodiscard]] ionshared::OptPtr<Statement> findLastStatement() noexcept;

        [[nodiscard]] ionshared::OptPtr<Function> findParentFunction();
    };
}
