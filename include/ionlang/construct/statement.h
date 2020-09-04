#pragma once

#include <ionshared/misc/helpers.h>
#include <ionlang/construct/pseudo/child_construct.h>

namespace ionlang {
    class Pass;

    class Block;

    typedef ChildConstructOpts<Block> StatementOpts;

    enum class StatementKind {
        If,

        Return,

        VariableDeclaration,

        Assignment,

        Call
    };

    class Statement : public ChildConstruct<Block> {
    private:
        StatementKind kind;

        ionshared::OptPtr<Statement> yields;

    public:
        Statement(
            ionshared::Ptr<Block> parent,
            StatementKind kind,
            ionshared::OptPtr<Statement> yields = std::nullopt
        );

        virtual void accept(Pass &visitor) = 0;

        [[nodiscard]] StatementKind getStatementKind() const noexcept;

        [[nodiscard]] ionshared::OptPtr<Statement> getYields() const noexcept;

        [[nodiscard]] bool isTerminal() const noexcept;

        [[nodiscard]] uint32_t getOrder();
    };
}
