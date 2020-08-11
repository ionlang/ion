#pragma once

#include <string>
#include <ionshared/misc/named.h>
#include <ionshared/misc/helpers.h>
#include <ionshared/tracking/symbol_table.h>
#include <ionshared/tracking/scope_anchor.h>
#include <ionlang/construct/pseudo/child_construct.h>
#include "variable_declaration.h"
#include "statement.h"

namespace ionlang {
    class Pass;

    class FunctionBody;

    class StatementBuilder;

    enum class BasicBlockKind {
        /**
         * The entry basic block of a function body.
         */
        Entry,

        /**
         * A basic block which forms part of a construct. Cannot be
         * directly accessed by the user.
         */
        Internal
    };

    struct BasicBlockOpts : ChildConstructOpts<FunctionBody> {
        BasicBlockKind kind;

        std::string id;

        std::vector<ionshared::Ptr<VariableDeclaration>> variableDecls = {};

        std::vector<ionshared::Ptr<Statement>> statements = {};

        ionshared::PtrSymbolTable<Statement> symbolTable = ionshared::Util::makePtrSymbolTable<Statement>();
    };

    // TODO: Must be verified to contain a single terminal instruction at the end.
    class BasicBlock : public ChildConstruct<FunctionBody>, public ionshared::ScopeAnchor<Statement>, public ionshared::Named {
    private:
        BasicBlockKind kind;

        std::vector<ionshared::Ptr<VariableDeclaration>> variableDecls;

        std::vector<ionshared::Ptr<Statement>> statements;

    public:
        explicit BasicBlock(const BasicBlockOpts &opts);

        void accept(Pass &visitor) override;

        Ast getChildNodes() override;

        BasicBlockKind getKind() const noexcept;

        std::vector<ionshared::Ptr<VariableDeclaration>> &getRegisters() noexcept;

        void setVariableDecls(std::vector<ionshared::Ptr<VariableDeclaration>> variableDecls);

        std::vector<ionshared::Ptr<Statement>> &getStatements() noexcept;

        void setStatements(std::vector<ionshared::Ptr<Statement>> statements);

        void insertStatement(const ionshared::Ptr<Statement> &statement);

        uint32_t relocateStatements(BasicBlock &target, uint32_t from = 0);

        /**
         * Attempt to find the index location of an instruction.
         * Returns null if not found.
         */
        std::optional<uint32_t> locate(ionshared::Ptr<Statement> statement) const;

        ionshared::Ptr<StatementBuilder> createBuilder();

        ionshared::OptPtr<Statement> findTerminalStatement() const;
    };
}
