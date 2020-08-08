#pragma once

#include <ionshared/misc/named.h>
#include <ionshared/misc/helpers.h>
#include <string>

namespace ionlang {
    class Pass;

    class FunctionBody;

    class InstBuilder;

    enum class BasicBlockKind {
        /**
         * The entry basic block of a function body.
         */
        Entry,

        /**
         * A basic block defined by the user. Can be jumped to
         * using a goto instruction.
         */
        Label,

        /**
         * A basic block which forms part of a construct. Cannot be
         * directly accessed by the user.
         */
        Internal
    };

    struct BasicBlockOpts : ChildConstructOpts<FunctionBody> {
        BasicBlockKind kind;

        std::string id;

        std::vector<ionshared::Ptr<RegisterAssign>> registers = {};

        std::vector<ionshared::Ptr<Inst>> insts = {};

        PtrSymbolTable<Inst> symbolTable = TypeFactory::makePtrSymbolTable<Inst>();
    };

    // TODO: Must be verified to contain a single terminal instruction at the end.
    class BasicBlock : public ChildConstruct<FunctionBody>, public ScopeAnchor<Inst>, public ionshared::Named {
    private:
        BasicBlockKind kind;

        std::vector<ionshared::Ptr<RegisterAssign>> variableDecls;

        std::vector<ionshared::Ptr<Inst>> statements;

    public:
        explicit BasicBlock(BasicBlockOpts opts);

        void accept(Pass &visitor) override;

        Ast getChildNodes() override;

        BasicBlockKind getKind() const noexcept;

        std::vector<ionshared::Ptr<RegisterAssign>> &getRegisters() noexcept;

        void setVariableDecls(std::vector<ionshared::Ptr<RegisterAssign>> variableDecls);

        std::vector<ionshared::Ptr<Inst>> &getStatements() noexcept;

        void setStatements(std::vector<ionshared::Ptr<Inst>> statements);

        void insertStatement(ionshared::Ptr<Inst> statement);

        uint32_t relocateStatements(BasicBlock &target, uint32_t from = 0);

        /**
         * Attempt to find the index location of an instruction.
         * Returns null if not found.
         */
        std::optional<uint32_t> locate(ionshared::Ptr<Inst> statement) const;

        ionshared::Ptr<InstBuilder> createBuilder();

        ionshared::OptPtr<Inst> findTerminalStatement() const;
    };
}
